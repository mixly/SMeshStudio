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

#include "fsl_os_abstraction.h"
#include "wait_api.h"

fsl_rtos_status lock_destroy(lock_object_t *obj) {
    (void) (obj);
    return kSuccess;
}


fsl_rtos_status event_set(event_object_t *obj, event_group_t flags) {
    (void) obj, (void) flags;
    return kSuccess;
}

void time_delay(uint32_t delay) {
    wait_ms(delay);
}
