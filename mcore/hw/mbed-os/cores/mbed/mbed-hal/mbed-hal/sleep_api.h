/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#ifndef MBED_SLEEP_API_H
#define MBED_SLEEP_API_H

#include "device.h"

#if DEVICE_SLEEP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sleep_s sleep_t;

/** Enter a sleep mode.
 *
 * @param obj The sleep object which stores required data to restore from sleep
 */
void mbed_enter_sleep(sleep_t *obj);

/** Restore the mcu from the entered sleep mode.
 *
 * @param obj The sleep object which stores required data to restore from sleep
 */
void mbed_exit_sleep(sleep_t *obj);

#ifdef __cplusplus
}
#endif

#endif

#endif
