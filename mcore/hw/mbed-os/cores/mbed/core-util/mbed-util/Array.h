/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MBED_UTIL_ARRAY_H__
#define __MBED_UTIL_ARRAY_H__

#include <stddef.h>
#include <stdint.h>
#include "mbed-util/CriticalSectionLock.h"
#include "mbed-util/PoolAllocator.h"
#include "mbed-util/mbed-util.h"
#include "mbed-alloc/ualloc.h"

namespace mbed {
namespace util {

/** A reentrant Array class (elements can be accessed by index). It holds copies of the given type (T).
  *
  * The 'push_back' function can be used to add new entries to the array. If there's not enough space
  * available, the Array will try to allocate more memory and grow automatically. The array can only
  * grow, never shrink.
  *
  * This is not a sparse array. Trying to access elements outside the current array size will result
  * in a runtime error or cause undefined behaviour.
  *
  * If the templated type is a class or a struct, it needs to have a copy constructor
  */
template <typename T>
class Array {
public:
    /** Create a new array
      */
    Array(): _head(NULL) {
    }

    ~Array() {
        // We have copies of elements of type T in our array, so destroy them first
        for (unsigned i = 0; i < _elements; i ++) {
            T *p = get_element_address(i);
            p->~T();
        }
        // Now it's safe to destroy our internal data structures
        array_link *crt = _head, *prev;
        while (crt != NULL) {
            prev = crt->prev;
            void *addr = crt->data;
            crt->~array_link(); // not really needed, just for completion
            mbed_ufree(addr);
            crt = prev;
        }
    }

    /** Initialize the array
      * @param initial_capacity initial number of elements in the array
      * @param grow_capacity number of elements to add when the array runs out of memory
      * @param alloc_traits allocator traits (for mbed_ualloc)
      * @param alignment alignment of each element in the array
      * @returns true if the initialization succeeded, false otherwise
      */
    bool init(size_t initial_capacity, size_t grow_capacity, UAllocTraits_t alloc_traits, unsigned alignment = MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN) {
        if (_head != NULL)
            return false; // prevent repeated initialization
        _element_size = PoolAllocator::align_up(sizeof(T), alignment);
        _grow_capacity = grow_capacity;
        _alloc_traits = alloc_traits;
        _alignment = alignment;
        _capacity = initial_capacity;
        _elements = 0;
        _head = create_new_array(initial_capacity);
        return _head != NULL;
    }

    /** Subscript operator: return a reference to an existing element
      * Calling this function with an invalid index results in undefined behaviour!
      * @param index element index
      * @returns reference to the element at 'idx'
      */
    T& operator [](unsigned idx) {
        return *get_element_address(idx);
    }

    /** Subscript operator: return a reference to an existing element (const version)
      * Calling this function with an invalid index results in undefined behaviour!
      * @param index element index
      * @returns constreference to the element at 'idx'
      */
    const T& operator [](unsigned idx) const {
        return *get_element_address(idx);
    }

    /** Return a reference to an existing element
      * Calling this function with an invalid index results in a runtime error.
      * @param index element index
      * @returns reference to the element at 'idx'
      */
    T& at(unsigned idx) {
        check_access(idx);
        return *get_element_address(idx);
    }

    /** Return a reference to an existing element (const version)
      * Calling this function with an invalid index results in a runtime error.
      * @param index element index
      * @returns const reference to the element at 'idx'
      */
    const T& at(unsigned idx) const {
        check_access(idx);
        return *get_element_address(idx);
    }

    /** Adds an element at the end of the array
      * If there's not enough memory for a new element, a new zone will be allocated
      * @param new_element element to add
      * @returns true if the element was added, false otherwise (out of memory)
      */
    bool push_back(const T& new_element) {
        unsigned idx = _elements, prev_capacity = _capacity;
        if (_elements == _capacity) {
            if (_grow_capacity == 0) { // can we grow?
                return false;
            }
            {
                CriticalSectionLock lock;
                if (prev_capacity == _capacity) { // allocate only if someone else didn't 
                    if((_head = create_new_array(_grow_capacity, _elements, _head)) == NULL) {
                        return false;
                    }
                    _capacity += _grow_capacity;
                }
            }
        }
        {
            // TODO: this will eventually be 'atomic_set(&_elements);' after we figure
            // out the atomic access primitives
            CriticalSectionLock lock;
            _elements ++;
        }
        new(get_element_address(idx)) T(new_element);
        return true;
    }

    /** Removes the last element in the array
      */
    void pop_back() {
        T *p = NULL;
        {
            CriticalSectionLock lock;
            if (_elements > 0) {
                p = get_element_address(_elements - 1);
                --_elements;
            }
        }
        if (p != NULL) {
            p->~T();
        }
    }

    /** Return the number of zones (linked memory areas) in this array
      * @returns number of zones
      */
    unsigned get_num_zones() const {
        unsigned cnt = 0;
        array_link *crt = _head;

        while (crt != NULL) {
            cnt ++;
            crt = crt->prev;
        }
        return cnt;
    }

    /** Returns the number of elements in the array
      * @returns number of elements
      */
    unsigned get_num_elements() const {
        return _elements;
    }

    /** Returns the capacity of the array
      * @returns capacity of the array
      */
    unsigned get_capacity() const {
        return _capacity;
    }

private:
    struct array_link {
        array_link(void *_data, unsigned _first_idx, array_link *_prev):
            data((uint8_t*)_data),
            first_idx(_first_idx),
            prev(_prev) {
        }

        uint8_t *data;
        unsigned first_idx;
        array_link *prev;
    };

    array_link *create_new_array(size_t elements, unsigned first_idx = 0, array_link *prev = NULL) const {
        // Create the array space + an array_link structure in the same contigous memory area
        // Layout: array storage area | array_link structure
        // Since the array elements are aligned to at least 4 bytes, the array_link address will be correctly aligned
        size_t array_storage_size = _element_size * elements;
        void *temp = mbed_ualloc(array_storage_size + sizeof(array_link), _alloc_traits);
        if (temp == NULL)
            return NULL;
        array_link *p = new((char*)temp + array_storage_size) array_link(temp, first_idx, prev);
        return p;
    }

    T *get_element_address(unsigned idx) const {
        array_link *crt = _head;

        MBED_UTIL_ASSERT(idx < _elements);
        // first_idx in the first allocated array is 0, so the loop below is guaranteed to end
        while(idx < crt->first_idx) {
            crt = crt->prev;
        }
        MBED_UTIL_ASSERT(crt != NULL);
        return (T*)(crt->data + _element_size * (idx - crt->first_idx));
    }

    void check_access(unsigned idx) const {
        if (NULL == _head) {
            MBED_UTIL_RUNTIME_ERROR("Attempt to use uninitialized Array %p\r\n", this);
        }
        if (idx >= _elements) {
            MBED_UTIL_RUNTIME_ERROR("Attempt to use invalid index %u in Array %p\r\n", idx, this);
        }
    }

    array_link *volatile _head;
    UAllocTraits_t _alloc_traits;
    size_t _element_size, _grow_capacity;
    volatile unsigned _capacity, _elements;
    unsigned _alignment;
};

} // namespace util
} // namespace mbed

#endif // #ifndef __MBED_UTIL_ARRAY_H__

