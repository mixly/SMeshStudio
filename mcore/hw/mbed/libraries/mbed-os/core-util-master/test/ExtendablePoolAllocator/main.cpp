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
#include "mbed/test_env.h"
#include "mbed-alloc/ualloc.h"
#include <stdio.h>
#include <stdlib.h>

using namespace mbed::util;

static bool check_value_and_alignment(void *p, unsigned alignment = MBED_UTIL_POOL_ALLOC_DEFAULT_ALIGN) {
    if (NULL == p)
        return false;
    return ((uint32_t)p & (alignment - 1)) == 0;
}

void app_start(int, char**) {
    MBED_HOSTTEST_TIMEOUT(5);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(mbed-util extendable pool allocator test);
    MBED_HOSTTEST_START("MBED_UTIL_EXTENDABLE_POOL_ALLOCATOR_TEST");

    // Create the allocator
    const size_t initial_elements = 20, new_pool_elements = 12, element_size = 6;
    UAllocTraits_t traits = {0};
    ExtendablePoolAllocator allocator;
    MBED_HOSTTEST_ASSERT(allocator.init(initial_elements, new_pool_elements, element_size, traits));

    // Fill the first pool
    for (unsigned i = 0; i < initial_elements; i ++) {
        MBED_HOSTTEST_ASSERT(check_value_and_alignment(allocator.alloc()));
    }
    MBED_HOSTTEST_ASSERT(allocator.get_num_pools() == 1);

    // Allocating just another element should add another pool
    void *p = allocator.alloc();
    MBED_HOSTTEST_ASSERT(check_value_and_alignment(p));
    MBED_HOSTTEST_ASSERT(allocator.get_num_pools() == 2);
    // Fill the new pool
    for (unsigned i = 0; i < new_pool_elements - 1; i ++) {
        MBED_HOSTTEST_ASSERT(check_value_and_alignment(allocator.alloc()));
    }
    MBED_HOSTTEST_ASSERT(allocator.get_num_pools() == 2);

    // Free one previously allocated area
    allocator.free(p);
    // And allocate again. Since there's a single free place, it should be used now
    void *newp = allocator.alloc();
    MBED_HOSTTEST_ASSERT(check_value_and_alignment(newp));
    MBED_HOSTTEST_ASSERT(newp == p);
    MBED_HOSTTEST_ASSERT(allocator.get_num_pools() == 2);

    MBED_HOSTTEST_RESULT(true);
}

