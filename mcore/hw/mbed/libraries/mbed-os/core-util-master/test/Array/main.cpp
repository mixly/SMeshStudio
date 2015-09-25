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

#include "core-util/Array.h"
#include "mbed/test_env.h"
#include <stdio.h>
#include <stdlib.h>

using namespace mbed::util;

static void test_pod() {
    Array<unsigned> array;

    const size_t initial_capacity = 20, grow_capacity = 12, alignment = 4;
    UAllocTraits_t traits = {0};
    MBED_HOSTTEST_ASSERT(array.init(initial_capacity, grow_capacity, traits, alignment));

    // Start filling the array
    for (unsigned i = 0; i < initial_capacity; i ++ ) {
        array.push_back(i);
    }
    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity);
    for (unsigned i = 0; i < initial_capacity; i ++) {
        MBED_HOSTTEST_ASSERT(array[i] == i);
    }
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 1);

    // Add another element, this should trigger the creation of another zone
    array.push_back(1000);
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 2);
    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity + 1);
    // Fill the second zone too
    for (unsigned i = 1; i < grow_capacity; i ++) {
        array.push_back(1000 + i);
    }
    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity + grow_capacity);
    for (unsigned i = 0; i < grow_capacity; i ++) {
        MBED_HOSTTEST_ASSERT(array.at(i + initial_capacity) == 1000 + i);
    }
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 2);
    unsigned save_for_later = array[initial_capacity + grow_capacity - 1];
    // Add yet another element, which should result in the creation of another zone
    array.push_back(10000);
    MBED_HOSTTEST_ASSERT(array[initial_capacity + grow_capacity] == 10000);
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 3);
    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity + grow_capacity + 1);

    // Remove the last element
    array.pop_back();
    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity + grow_capacity);
    MBED_HOSTTEST_ASSERT(array[array.get_num_elements() - 1] == save_for_later);
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 3); // the array doesn't (yet?) shrink

    // Simple bubble sort test illustrating moving around elements in the array
    const size_t total = initial_capacity + grow_capacity;
    for (unsigned i = 0; i < total; i ++) {
        array.at(i) = total - i - 1;
    }

    for (unsigned i = 0; i < total - 1; i ++) {
        for (unsigned j = i + 1; j < total; j ++) {
            if (array[i] > array[j]) {
                unsigned temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    for (unsigned i = 0; i < total; i ++) {
        MBED_HOSTTEST_ASSERT(array[i] == i);
    }

    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 3);
}

struct Test {
    Test(unsigned a = 0, uint8_t c = 0): _a(a), _c(c) {
        inst_count ++;
    }

    Test(const Test& t): _a(t._a), _c(t._c) {
        inst_count ++;
    }

    ~Test() {
        inst_count --;
    }

    bool operator ==(const Test& t) const {
        return (t._a == _a) && (t._c == _c);
    }

    unsigned _a;
    uint8_t _c;
    static int inst_count;
};

int Test::inst_count = 0;

static void test_non_pod() {
    Array<Test> array;

    const size_t initial_capacity = 10, grow_capacity = 6, alignment = 4;
    UAllocTraits_t traits = {0};
    MBED_HOSTTEST_ASSERT(array.init(initial_capacity, grow_capacity, traits, alignment));

    // Just fill the first part of the array and verify
    unsigned idx;
    for (idx = 0; idx < initial_capacity; idx ++) {
        array.push_back(Test(idx, 'a'));
    }
    for (idx = 0; idx < initial_capacity; idx ++) {
        MBED_HOSTTEST_ASSERT(array[idx] == Test(idx, 'a'));
    }

    // Now override what we wrote with different data
    for (idx = 0; idx < initial_capacity; idx ++) {
        array[idx] = Test(idx, 'b');
    }
    for (idx = 0; idx < initial_capacity; idx ++) {
        const Test& t = array[idx];
        MBED_HOSTTEST_ASSERT(t == Test(idx, 'b'));
    }

    // Pop the last element from array (checks if destructor is called)
    array.pop_back();

    MBED_HOSTTEST_ASSERT(array.get_num_elements() == initial_capacity - 1);
    MBED_HOSTTEST_ASSERT(array.get_num_zones() == 1);
}

void app_start(int, char**) {
    MBED_HOSTTEST_TIMEOUT(5);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(mbed-util array  test);
    MBED_HOSTTEST_START("MBED_UTIL_ARRAY_TEST");

    test_pod(); // test with "plain old data"
    test_non_pod(); // test with complex data
    MBED_HOSTTEST_ASSERT(Test::inst_count == 0);

    MBED_HOSTTEST_RESULT(true);
}

