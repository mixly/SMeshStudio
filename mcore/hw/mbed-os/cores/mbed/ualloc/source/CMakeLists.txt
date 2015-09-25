# mbed Microcontroller Library
# Copyright (c) 2015 ARM Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# this custom build file pulls in different linker commands based on the
# toolchain being used

if(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(LINK_OVERRIDE_FLAGS "-Wl,-wrap,_malloc_r -Wl,-wrap,_calloc_r -Wl,-wrap,_realloc_r -Wl,-wrap,_free_r -Wl,-wrap,_sbrk_r")
elseif(${CMAKE_C_COMPILER_ID} STREQUAL ARMCC)
    set(LINK_OVERRIDE_FLAGS "--undefined=mbed_ualloc --undefined=malloc_c_force_import")
else()
    set(LINK_OVERRIDE_FLAGS "")
    message(FATAL_ERROR "Unsupported target toolchain (gcc supported)")
endif()

add_library(ualloc malloc.c ualloc.c)
target_link_libraries(ualloc dlmalloc ${LINK_OVERRIDE_FLAGS})
