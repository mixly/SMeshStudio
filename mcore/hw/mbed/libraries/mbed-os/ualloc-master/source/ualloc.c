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
#include "mbed-alloc/ualloc.h"
#include "core-util/sbrk.h"
#include "string.h"

extern void* dlmalloc(size_t);
extern void  dlfree(void*);
extern void* dlcalloc(size_t, size_t);
extern void* dlrealloc(void*, size_t);

extern int printf(const char *, ...);

extern void * volatile mbed_sbrk_ptr;

// Set debug level to 0 until non-allocating printf is available
const UAllocDebug_t ualloc_debug_level = UALLOC_DEBUG_NONE;//(DEBUG?UALLOC_DEBUG_MAX:UALLOC_DEBUG_NONE);

const char ua_chars[] = "NFEWIL";

#define ualloc_debug(ADBG_LEVEL, fmt, ...) do {                              \
    if (ADBG_LEVEL <= ualloc_debug_level && ADBG_LEVEL < UALLOC_DEBUG_MAX) { \
        printf("UAL:%c " fmt, ua_chars[ADBG_LEVEL], __VA_ARGS__);            \
    }                                                                        \
} while (0)


#if defined(__ARMCC_VERSION)
    #define caller_addr() __builtin_return_address(0)
#elif defined(__GNUC__)
    #define caller_addr() __builtin_extract_return_addr(__builtin_return_address(0))
#else
    #define caller_addr() (NULL)
#endif

void * mbed_ualloc(size_t bytes, UAllocTraits_t traits)
{
    void * ptr = NULL;
    void * caller = (void*) caller_addr();
    if (UALLOC_TEST_TRAITS(traits.flags, UALLOC_TRAITS_NEVER_FREE)) {
        ptr = mbed_krbs(bytes);
        if ((ptr != NULL) && UALLOC_TEST_TRAITS(traits.flags, UALLOC_TRAITS_ZERO_FILL)) {
            memset(ptr, 0, bytes);
        }
    } else if (UALLOC_TEST_TRAITS(traits.flags, UALLOC_TRAITS_ZERO_FILL)) {
        ptr = dlcalloc(1, bytes);
    } else if (!(traits.flags & ~UALLOC_TRAITS_BITMASK)) {
        ptr = dlmalloc(bytes);
    } else if (traits.flags & UALLOC_RESERVED_MASK) {
        ualloc_debug(UALLOC_DEBUG_ERROR, "ua c:%p reserved: %lx\n", caller,
            traits.flags & UALLOC_RESERVED_MASK);
    }

    if(ptr == NULL) {
        ualloc_debug(UALLOC_DEBUG_WARNING, "ua c:%p fail\n", caller);
    } else {
        ualloc_debug(UALLOC_DEBUG_LOG, "ua c:%p m:%p\n", caller, ptr);
    }
    return ptr;
}
void * mbed_urealloc(void * ptr, size_t bytes, UAllocTraits_t traits)
{
    void * caller = (void*) caller_addr();
    void *newptr = NULL;
    if(traits.flags & ~UALLOC_TRAITS_BITMASK) {
        // Traits not supported in urealloc yet
        ualloc_debug(UALLOC_DEBUG_WARNING, "ua c:%p fail\n", caller);
        return NULL;
    }
    uintptr_t ptr_tmp = (uintptr_t) ptr;
    if ((ptr_tmp < (uintptr_t) mbed_sbrk_ptr) &&
            (ptr_tmp >= (uintptr_t)&__mbed_sbrk_start)) {
        newptr = dlrealloc(ptr, bytes);
    } else {
        ualloc_debug(UALLOC_DEBUG_LOG, "uf c:%p m:%p non-heap realloc\n", caller, ptr);
    }

    if(newptr == NULL) {
        ualloc_debug(UALLOC_DEBUG_WARNING, "ur c:%p m0:%p fail\n", caller, ptr);
    } else {
        ualloc_debug(UALLOC_DEBUG_LOG, "ur c:%p m0:%p m1:%p\n", caller, ptr, newptr);
    }
    return newptr;
}
void mbed_ufree(void * ptr)
{
    void * caller = (void*) caller_addr();
    ualloc_debug(UALLOC_DEBUG_LOG, "uf c:%p m:%p\n", caller, ptr);
    uintptr_t ptr_tmp = (uintptr_t) ptr;
    if ((ptr_tmp < (uintptr_t) mbed_sbrk_ptr) &&
            (ptr_tmp >= (uintptr_t)&__mbed_sbrk_start)) {
        dlfree(ptr);
    } else {
        ualloc_debug(UALLOC_DEBUG_LOG, "uf c:%p m:%p non-heap free\n", caller, ptr);
    }
}
