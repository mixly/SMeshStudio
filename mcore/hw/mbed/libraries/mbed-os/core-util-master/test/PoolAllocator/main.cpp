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

#include "core-util/PoolAllocator.h"
#include "mbed/test_env.h"
#include <stdio.h>
#include <stdlib.h>

using namespace mbed::util;

void app_start(int, char**) {
    MBED_HOSTTEST_TIMEOUT(5);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(mbed-util pool allocator test);
    MBED_HOSTTEST_START("MBED_UTIL_POOL_ALLOCATOR_TEST");

    // Allocate initial space for the pool
    const size_t elements = 10, element_size = 6;
    const size_t aligned_size = (element_size + MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN - 1) & ~(MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN - 1);
    size_t pool_size = PoolAllocator::get_pool_size(elements, element_size);
    MBED_HOSTTEST_ASSERT(pool_size == elements * aligned_size);

    void *start = malloc(pool_size);
    MBED_HOSTTEST_ASSERT(start != NULL);
    PoolAllocator allocator(start, elements, element_size);

    // Allocate all elements, checking for proper alignment and spacing
    void *p, *prev, *first;
    for (size_t i = 0; i < elements; i ++) {
        p = allocator.alloc();
        MBED_HOSTTEST_ASSERT(p != NULL);
        // Check alignment
        MBED_HOSTTEST_ASSERT(((uint32_t)p & (MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN - 1)) == 0);
        // Check spacing
        if (i > 0) {
            MBED_HOSTTEST_ASSERT(((uint32_t)p - (uint32_t)prev) == aligned_size);
        } else {
            first = p;
            MBED_HOSTTEST_ASSERT(p == start);
        }
        prev = p;
    }

    // No more space in the pool, we should get NULL now
    MBED_HOSTTEST_ASSERT(allocator.alloc() == NULL);

    // Free the first element we allocated
    allocator.free(first);

    // Verify that we can allocate a single element now, and it has the same address
    // as the first element we allocated above
    p = allocator.alloc();
    MBED_HOSTTEST_ASSERT(p == first);
    p = allocator.alloc();
    MBED_HOSTTEST_ASSERT(p == NULL);

    MBED_HOSTTEST_RESULT(true);
}

