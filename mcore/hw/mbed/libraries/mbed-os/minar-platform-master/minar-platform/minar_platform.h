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

#ifndef __MINAR_PLATFORM_MINAR_PLATFORM_H__
#define __MINAR_PLATFORM_MINAR_PLATFORM_H__

#include <stdint.h>
#include "minar-platform/minar_platform_types.h"

namespace minar {
namespace platform {

/// @name Initialisation
void init();


/// @name Energy Management
void sleep();
void sleepFromUntil(tick_t now, tick_t until);


/// @name Time
tick_t getTime();
uint32_t getTimeOverflows();
uint32_t getTimeInMilliseconds();

/// @name IRQ State Management
irqstate_t pushDisableIRQState();
void popDisableIRQState(irqstate_t);

}; // namespace platform
}; // namespace minar

#endif // ndef __MINAR_PLATFORM_MINAR_PLATFORM_H__
