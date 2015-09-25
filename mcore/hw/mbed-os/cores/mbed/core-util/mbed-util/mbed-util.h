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

#ifndef __MBED_UTIL_MBED_UTIL_H__
#define __MBED_UTIL_MBED_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define MBED_UTIL_ASSERT(expr)              ((void)0)
#define MBED_UTIL_ASSERT_MSG(expr, msg)     ((void)0)
#else
#define MBED_UTIL_ASSERT(expr)                           \
do {                                                     \
    if (!(expr)) {                                       \
        mbed_util_assert_internal(#expr, __FILE__, __LINE__, NULL); \
    }                                                    \
} while (0)
#define MBED_UTIL_ASSERT_MSG(expr, msg)                  \
do {                                                     \
    if (!(expr)) {                                       \
        mbed_util_assert_internal(#expr, __FILE__, __LINE__, msg); \
    }                                                    \
} while (0)

#endif

#define MBED_UTIL_RUNTIME_ERROR(...)        mbed_util_runtime_error_internal(__FILE__, __LINE__, __VA_ARGS__)

void mbed_util_runtime_error_internal(const char *fname, int line, const char* fmt, ...);
void mbed_util_assert_internal(const char* expr, const char *fname, int line, const char *msg);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __MBED_UTIL_MBED_UTIL_H__

