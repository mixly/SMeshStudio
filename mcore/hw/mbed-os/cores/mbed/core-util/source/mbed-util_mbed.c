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

#ifdef TARGET_LIKE_MBED // only include this code for mbed targets

#include <stdlib.h>
#include <stdarg.h>
#include "device.h"
#include "mbed/mbed_interface.h"
#include "mbed-util/mbed-util.h"
#if DEVICE_STDIO_MESSAGES
#include <stdio.h>
#endif

void mbed_util_runtime_error_internal(const char *file, int line, const char* format, ...) {
#if DEVICE_STDIO_MESSAGES
    fprintf(stderr, "Runtime error in file %s, line %d: ", file, line);
    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
#endif
    exit(1);
}

void mbed_util_assert_internal(const char *expr, const char *file, int line, const char* msg)
{
#if DEVICE_STDIO_MESSAGES
    fprintf(stderr, "assertation failed: %s, file: %s, line %d", expr, file, line);
    if (msg)
        fprintf(stderr, " (%s)", msg);
    fprintf(stderr, "\r\n");
#endif
    mbed_die();
}

#endif // #ifdef TARGET_LIKE_MBED

