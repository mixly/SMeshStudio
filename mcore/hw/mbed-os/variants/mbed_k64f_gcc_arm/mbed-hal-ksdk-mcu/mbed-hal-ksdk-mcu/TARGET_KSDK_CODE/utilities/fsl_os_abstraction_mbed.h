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

#ifndef FSL_OS_ABSTRACTION_MBED_H_
#define FSL_OS_ABSTRACTION_MBED_H_

// This is not really an "abstraction", but rather a set of quick&dirty
// defines to allow the KSDK to compile. Currently, this is relevant only
// in the context of the ENET driver (fsl_enet_driver.c)

typedef int event_object_t;
typedef int lock_object_t;
typedef void sync_object_t;
typedef unsigned int event_group_t;
typedef int task_t;
typedef void task_stack_t;
typedef int task_handler_t;
typedef int msg_queue_handler_t;
typedef void msg_queue_t;
typedef int msg_queue_item_t;

#endif // #ifdef FSL_OS_ABSTRACTION_MBED_H_
