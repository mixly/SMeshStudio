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
#include <stddef.h>
#include <stdint.h>
#include "mbed/test_env.h"
#include "mbed-util/sbrk.h"
#include "mbed-alloc/ualloc.h"


extern void * volatile mbed_sbrk_ptr;
extern void * volatile mbed_krbs_ptr;
extern volatile uintptr_t mbed_sbrk_diff;

#define TEST_SIZE_0 0x20

void app_start(int, char**) {
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(ualloc zone test);
    MBED_HOSTTEST_START("UALLOC_ZONE");
    const char * current_test = "none";
    UAllocTraits_t traits;

    uintptr_t heap_start = reinterpret_cast<uintptr_t>(&__mbed_sbrk_start);
    uintptr_t heap_end   = reinterpret_cast<uintptr_t>(&__mbed_krbs_start);

    bool tests_pass = false;

    do {
        uintptr_t sbrk_ptv, krbs_ptv;
        void * ptr;
        // Make sure that stack variables are not on the heap
        current_test = "stack/heap";
        uint32_t testval;
        uintptr_t ptv = reinterpret_cast<uintptr_t>(&testval);
        if ((ptv > heap_start) && (ptv < heap_end)) {
            break;
        }

        // malloc
        current_test = "malloc";
        ptr = malloc(TEST_SIZE_0);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        // free
        free(ptr);

        // calloc
        current_test = "calloc";
        ptr = calloc(TEST_SIZE_0,4);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        // free
        free(ptr);

        // realloc
        current_test = "realloc";
        // malloc two blocks
        void * ptr0 = malloc(TEST_SIZE_0);
        if (ptr0 == NULL) {break;}
        void * ptr1 = malloc(TEST_SIZE_0);
        if (ptr1 == NULL) {break;}
        // realloc the first one and make sure the pointer moves
        void * ptr2 = realloc(ptr0, 2*TEST_SIZE_0);
        if (ptr2 == NULL) {break;}
        if (ptr2 == ptr0) {break;}
        // free
        free(ptr1);
        free(ptr2);

        // ualloc
        current_test = "ualloc no_flags";
        traits.flags = 0;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        mbed_ufree(ptr);

        traits.flags = UALLOC_TRAITS_BITMASK;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }
        mbed_ufree(ptr);

        current_test = "ualloc never_free";
        traits.flags = UALLOC_TRAITS_NEVER_FREE;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        ptv = reinterpret_cast<uintptr_t>(ptr);
        krbs_ptv = reinterpret_cast<uintptr_t>(mbed_krbs_ptr);
        if (!((ptv >= krbs_ptv) && (ptv < heap_end))) {
            break;
        }
        // this call is ignored, but should generate a debug message
        mbed_ufree(ptr);

        current_test = "ualloc zero-fill";
        traits.flags = UALLOC_TRAITS_BITMASK;
        // allocate a block
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);
        // Validate the allocation
        ptv = reinterpret_cast<uintptr_t>(ptr0);
        sbrk_ptv = reinterpret_cast<uintptr_t>(mbed_sbrk_ptr);
        if (!((ptv > heap_start) && (ptv < sbrk_ptv))) {
            break;
        }

        // Fill the memory
        memset(ptr, 0xA5, TEST_SIZE_0);
        // free the memory
        mbed_ufree(ptr0);
        // allocate the same size block with ualloc zero-fill
        traits.flags = UALLOC_TRAITS_ZERO_FILL;
        ptr1 = mbed_ualloc(TEST_SIZE_0, traits);

        // Validate that the pointers are the same
        if (ptr1 != ptr0) {
            break;
        }

        // Check for zero-fill
        bool ok = true;
        for (unsigned i = 0; ok && (i < TEST_SIZE_0/sizeof(uint32_t)); i++) {
            ok = ((uint32_t *)ptr1)[i] == 0;
        }
        if (!ok) {break;}

        mbed_ufree(ptr1);

        // never_free + zero-fill
        current_test = "never free/zero-fill";
        // prefill the memory
        memset((char*) mbed_krbs_ptr - TEST_SIZE_0, 0xA5, TEST_SIZE_0);
        // Allocate the memory
        traits.flags = UALLOC_TRAITS_ZERO_FILL | UALLOC_TRAITS_NEVER_FREE;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);

        ok = true;
        for (unsigned i = 0; ok && (i < TEST_SIZE_0/sizeof(uint32_t)); i++) {
            ok = ((uint32_t *)ptr)[i] == 0;
        }
        if (!ok) {break;}

        current_test = "urealloc";
        traits.flags = 0;
        // malloc two blocks
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr0 == NULL) {break;}
        ptr1 = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr1 == NULL) {break;}
        // realloc the first one and make sure the pointer moves
        ptr2 = mbed_urealloc(ptr0, 2 * TEST_SIZE_0, traits);
        if (ptr2 == NULL) {break;}
        if (ptr2 == ptr0) {break;}
        // free
        mbed_ufree(ptr1);
        mbed_ufree(ptr2);

        // Test failing allocations
        // try to realloc a never-free pointer
        current_test = "urealloc/never-free";
        // Allocate the memory
        traits.flags = UALLOC_TRAITS_ZERO_FILL | UALLOC_TRAITS_NEVER_FREE;
        void * nfptr = mbed_ualloc(TEST_SIZE_0, traits);
        if (nfptr == NULL) {break;}
        traits.flags = 0;
        ptr = mbed_urealloc(nfptr, 2 * TEST_SIZE_0, traits);
        if (ptr != NULL) {break;}

        // try to allocate with reserved flags
        current_test = "ualloc/reserved flags";
        traits.flags = UALLOC_RESERVED_MASK | UALLOC_TRAITS_BITMASK;
        ptr = mbed_ualloc(TEST_SIZE_0, traits);
        if (ptr != NULL) {break;}

        // try urealloc with any flags
        current_test = "urealloc/flags";
        traits.flags = 0;
        // malloc two blocks
        ptr0 = mbed_ualloc(TEST_SIZE_0, traits);

        traits.flags = UALLOC_TRAITS_NEVER_FREE;
        ptr2 = mbed_urealloc(ptr0, 2 * TEST_SIZE_0, traits);
        if (ptr2 != NULL) {break;}
        // free
        mbed_ufree(ptr0);


        tests_pass = true;
    } while (0);

    if (!tests_pass) {
        printf("First failing test: %s\r\n", current_test);
    }

    MBED_HOSTTEST_RESULT(tests_pass);
}
