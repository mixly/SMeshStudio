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

#include "core-util/ExtendablePoolAllocator.h"
#include "core-util/PoolAllocator.h"
#include "core-util/CriticalSectionLock.h"
#include "mbed-alloc/ualloc.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <new>

namespace mbed {
namespace util {

ExtendablePoolAllocator::ExtendablePoolAllocator(): _head(NULL) {
}

bool ExtendablePoolAllocator::init(size_t initial_elements, size_t new_pool_elements, size_t element_size, UAllocTraits_t alloc_traits, unsigned alignment) {
    if (_head != NULL)
        return false; // don't initialize twice
    _new_pool_elements = new_pool_elements;
    _element_size = PoolAllocator::align_up(element_size, alignment);
    _alloc_traits = alloc_traits;
    _alignment = alignment;
    _head = create_new_pool(initial_elements, NULL);
    return _head != NULL;
}

ExtendablePoolAllocator::~ExtendablePoolAllocator() {
    pool_link *crt = _head, *prev;
    void *area;
    while (crt != NULL) {
        prev = crt->prev;
        area = crt->allocator.get_start_address();
        crt->~pool_link(); // this assumes that the PoolAllocator doesn't free its storage!
        mbed_ufree(area);
        crt = prev;
    }
}

void* ExtendablePoolAllocator::alloc() {
    // Try the current pool first
    if (NULL == _head)
        return NULL;
    void *blk = _head->allocator.alloc();
    if (blk != NULL)
        return blk;

    // Try all the other pools
    pool_link *prev_head = _head;
    pool_link *crt = prev_head->prev;
    while (crt != NULL) {
        if ((blk = crt->allocator.alloc()) != NULL) {
            return blk;
        }
        crt = crt->prev;
    }

    // Not enough space, need to create another pool
    {
        CriticalSectionLock lock; // execute with interrupts disabled
        if (_head != prev_head) { // if someone else already allocated a new pool, use it
            if ((blk = _head->allocator.alloc()) != NULL) {
                return blk;
            }
        }
        // Create a new pool and link it in the list of pools
        if ((crt = create_new_pool(_new_pool_elements, _head)) != NULL) {
            _head = crt;
            return crt->allocator.alloc();
        }
    }
    return NULL;
}

void *ExtendablePoolAllocator::calloc() {
    uint32_t *blk = (uint32_t*)alloc();

    if (blk == NULL)
        return NULL;
    for (unsigned i = 0; i < _element_size / 4; i ++, blk ++)
        *blk = 0;
    return blk;
}

void ExtendablePoolAllocator::free(void *p) {
    pool_link *crt = _head;

    // Delegate freeing to the pool that owns the pointer
    while (crt != NULL) {
        if (crt->allocator.owns(p)) {
            crt->allocator.free(p);
            return;
        }
        crt = crt->prev;
    }
}

unsigned ExtendablePoolAllocator::get_num_pools() const {
    pool_link *crt = _head;
    unsigned cnt = 0;

    while (crt != NULL) {
        cnt ++;
        crt = crt->prev;
    }
    return cnt;
}

ExtendablePoolAllocator::pool_link* ExtendablePoolAllocator::create_new_pool(size_t elements, pool_link *prev) const {
    // Create a pool instance + the actual pool space + a link to the previous pool allocator in the chain in a contigous memory area.
    // Layout: pool storage area | pool_link structure (pointer to previous pool and PoolAllocator instance)
    // Since the pool storage area aligns all the allocations internally to at least 4 bytes, the pool_link address will be correctly aligned
    size_t pool_storage_size = PoolAllocator::get_pool_size(elements, _element_size, _alignment);
    void *temp = mbed_ualloc(pool_storage_size + sizeof(pool_link), _alloc_traits);
    if (temp == NULL)
        return NULL;
    pool_link *p = new((char*)temp + pool_storage_size) pool_link(temp, elements, _element_size, _alignment, prev);
    return p;
}

} // namespace util
} // namespace mbed

