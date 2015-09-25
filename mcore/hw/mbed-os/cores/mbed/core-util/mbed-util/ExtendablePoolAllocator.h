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

#ifndef __MBED_UTIL_EXTENDABLE_POOL_ALLOCATOR_H__
#define __MBED_UTIL_EXTENDABLE_POOL_ALLOCATOR_H__

#include <stddef.h>
#include "mbed-util/PoolAllocator.h"
#include "mbed-alloc/ualloc.h"

namespace mbed {
namespace util {

/** A pool allocator that can grow automatically to satisfy allocation requests.
  *
  * The pool allocator contains a linked list of one or more PoolAllocator instances
  *
  * ExtendablePoolAllocator starts with a single PoolAllocator. Allocation is first
  * attempted from the most recent pool; if that fails, allocation is attempted again
  * from the other pools. If that fails, a new pool is created (with a number of elements
  * specified by 'set_new_pool_size' and allocation is attempted from this new pool
  */

class ExtendablePoolAllocator {
public:
    /** Create a new extendable pool allocator
      */
    ExtendablePoolAllocator();

    /** Destructor. It will automatically free all allocated memory
      */
    ~ExtendablePoolAllocator();

    /** Initialize the allocator, allocating the first pool
      * @param initial_elements the size of the initial pool in elements (each of element_size bytes)
      * @param new_pool_elements size of pools that are going to be created to satisfy
      *        allocation requests
      * @param element_size size of each pool element in bytes (this might be rounded up
               to satisfy the 'alignment' argument)
      * @param alloc_traits mbed_alloc traits for allocating the pools
      * @param alignment allocation alignment in bytes (must be a power of 2, at least 4)
      * @returns true if the initialization was OK, false otherwise
      */
    bool init(size_t initial_elements, size_t new_pool_elements, size_t element_size, UAllocTraits_t alloc_traits, unsigned alignment = MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN);

    /** Allocate a new element from the pool
      * It will try to allocate using the most recent pool
      * Failing that, it will try to allocate from all the other pools
      * Failing that, it will try to create a new pool and allocate from it
      * @returns the address of the new element or NULL for error
      */
    void* alloc();

    /** Allocate a new element from the pool and initialize it with 0
      * @returns the address of the new element or NULL for error
      */
    void *calloc();

    /** Free a previously allocated element
      * @param p pointer to element
      */
    void free(void* p);

    /** Return the number of PoolAllocator instances in this pool
      * @returns number of PoolAllocator instances
      */
    unsigned get_num_pools() const;

private:
    struct pool_link {
        pool_link(void *start, size_t elements, size_t element_size, unsigned alignment, pool_link *_prev):
            prev(_prev),
            allocator(start, elements, element_size, alignment) {
        }

        pool_link *prev;
        PoolAllocator allocator;
    };
    pool_link *create_new_pool(size_t elements, pool_link *prev) const;

    pool_link *volatile _head;
    size_t _element_size, _new_pool_elements;
    UAllocTraits_t _alloc_traits;
    unsigned _alignment;
};

} // namespace util
} // namespace mbed

#endif // #ifndef __MBED_UTIL_EXTENDABLE_POOL_ALLOCATOR_H__

