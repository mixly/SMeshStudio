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

#include <stdio.h>
#include <stdint.h>

#include "compiler-polyfill/attributes.h"

struct TestAttrPackedStruct{
    char a;
    int x;
} __packed;

int testPacked(){
    int failed = 0;

    if(sizeof(struct TestAttrPackedStruct) != sizeof(int) + sizeof(char)){
        printf("__packed test failed\n");
        failed++;
    }

    return failed;
}



struct TestAligned64Struct{
} __align(8);

struct TestAligned128Struct{
} __align(16);

int testAlign(){
    int failed = 0;

    struct TestAligned64Struct  a;
    struct TestAligned64Struct  b;
    struct TestAligned128Struct c;
    struct TestAligned64Struct  d;
    struct TestAligned128Struct e;
    
    if(((uintptr_t)&a) & 0x7){
        printf("__align 64 test 1 failed\n");
        failed++;
    }
    if(((uintptr_t)&b) & 0x7){
        printf("__align 64 test 2 failed\n");
        failed++;
    }
    if(((uintptr_t)&c) & 0xf){
        printf("__align 128 test 1 failed\n");
        failed++;
    }
    if(((uintptr_t)&d) & 0x7){
        printf("__align 64 test 3 failed\n");
        failed++;
    }
    if(((uintptr_t)&e) & 0xf){
        printf("__align 128 test 2 failed\n");
        failed++;
    }

    return failed;
}

int testUnused(int __unused arg){
    return 0;
}


int main(){
    int failed = 0;
    
    failed += testPacked();
    failed += testAlign();
    failed += testUnused(0);

    return failed;
}

