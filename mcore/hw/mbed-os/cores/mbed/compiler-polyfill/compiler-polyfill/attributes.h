/* compiler-polyfill
 * Copyright (c) 2014 ARM Limited
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
 
#ifndef __COMPILER_POLYFILL_ATTRIBUTES_H__
#define __COMPILER_POLYFILL_ATTRIBUTES_H__

#if defined(__GNUC__) || defined (__CC_ARM) || defined(__clang__)
// GCC, armcc and llvm/clang
    #ifndef __packed
        #define __packed __attribute__((packed))
    #endif
    
    #ifndef __align
        #define __align(N) __attribute__((aligned (N)))
    #endif

    #ifndef __unused
        #define __unused __attribute__((__unused__))
    #endif

    #ifndef __weak
        #define __weak __attribute__((weak))
    #endif
#else
// unknown compiler
    #error "this compiler is not yet supported by compiler-polyfill, if you can contribute support please submit a pull request at https://github.com/ARMmbed/compiler-polyfill"
#endif

#endif // ndef __COMPILER_POLYFILL_ATTRIBUTES_H__
