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

#include "mbed-util/PoolAllocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "mbed-util/atomic_ops.h"

namespace mbed {
namespace util {

PoolAllocator::PoolAllocator(void *start, size_t elements, size_t element_size, size_t alignment):
    _start(start), _element_size(element_size) {
    _element_size = align_up(element_size, alignment);
    _end = (void*)((uint8_t*)start + _element_size * elements);
    _init();
}

void* PoolAllocator::alloc() {
    uint32_t prev_free = (uint32_t)_free_block;
    if (0 == prev_free)
        return NULL;
    while (true) {
        void **const new_free = (void **)(*((void **)prev_free));
        if (atomic_cas((uint32_t*)&_free_block, &prev_free, (uint32_t)new_free)) {
            return (void*)prev_free;
        }
    }
}

void PoolAllocator::free(void* p) {
    if (owns(p)) {
        uint32_t prev_free = (uint32_t)_free_block;
        while (true) {
            *((void**)p) = (void*)prev_free;
            if (atomic_cas((uint32_t*)&_free_block, &prev_free, (uint32_t)p)) {
                break;
            }
        }
    }
}

bool PoolAllocator::owns(void *p) const {
    return (p >= _start) && (p < _end);
}

size_t PoolAllocator::get_pool_size(size_t elements, size_t element_size, unsigned alignment) {
    element_size = align_up(element_size, alignment);
    return element_size * elements;
}

void* PoolAllocator::calloc() {
    uint32_t *blk = (uint32_t*)alloc();

    if (NULL == blk)
        return NULL;
    for(unsigned i = 0; i < _element_size / 4; i ++, blk ++)
        *blk = 0;
    return blk;
}

uint32_t PoolAllocator:: align_up(uint32_t n, uint32_t alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

void* PoolAllocator::get_start_address() const {
    return _start;
}

void PoolAllocator::_init() {
    _free_block = _start;

    // Link all free blocks using offsets.
    void* next;
    void* blk = _free_block;

    while(true) {
        next = ((uint8_t *) blk) + _element_size;
        if(next > _end)
            break;
        *((void **)blk) = next;
        blk = next;
    }
    // count back one block and mark it as the last one
    blk = ((uint8_t *) blk) - _element_size;
    *((void **)blk) = 0;
}

} // namespace util
} // namespace mbed

