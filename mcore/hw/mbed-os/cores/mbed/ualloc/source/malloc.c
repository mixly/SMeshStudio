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
#include "mbed-util/sbrk.h"
#include <stdlib.h>

#if defined(__ARMCC_VERSION)
void * malloc_c_force_import;
void * $Sub$$malloc(size_t size) {
    UAllocTraits_t traits = {0};
    return mbed_ualloc(size, traits);
}
void * $Sub$$calloc(size_t elements, size_t size) {
    UAllocTraits_t traits = {UALLOC_TRAITS_ZERO_FILL};
    return mbed_ualloc(elements*size, traits);
}
void * $Sub$$realloc(void * ptr, size_t size) {
    UAllocTraits_t traits = {0};
    return mbed_urealloc(ptr, size, traits);
}
void $Sub$$free(void * ptr) {
    mbed_ufree(ptr);
}
#elif defined(__GNUC__)
void * __wrap__malloc_r(struct _reent *r, size_t size) {
    (void) r;
    UAllocTraits_t traits = {0};
    return mbed_ualloc(size, traits);
}
void * __wrap__calloc_r(struct _reent *r, size_t elements, size_t size) {
    (void) r;
    UAllocTraits_t traits = {UALLOC_TRAITS_ZERO_FILL};
    return mbed_ualloc(elements*size, traits);
}
void * __wrap__realloc_r(struct _reent *r, void * ptr, size_t size) {
    (void)r;
    UAllocTraits_t traits = {0};
    return mbed_urealloc(ptr, size, traits);
}
void __wrap__free_r(struct _reent *r, void * ptr) {
    (void)r;
    mbed_ufree(ptr);
}
void * __wrap__sbrk_r(struct _reent *r, ptrdiff_t size) {
    (void) r;
    return mbed_sbrk(size);
}
#endif
