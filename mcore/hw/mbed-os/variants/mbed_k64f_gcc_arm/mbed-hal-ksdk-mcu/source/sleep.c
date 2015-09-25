/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "sleep_api.h"
#include "cmsis.h"
#include "fsl_mcg_hal.h"
#include "fsl_smc_hal.h"
#include "objects.h"

//  ______ _______   ____  __ ______   _   _   _
// |  ____|_   _\ \ / /  \/  |  ____| | | | | | |
// | |__    | |  \ V /| \  / | |__    | | | | | |
// |  __|   | |   > < | |\/| |  __|   | | | | | |
// | |     _| |_ / . \| |  | | |____  |_| |_| |_|
// |_|    |_____/_/ \_\_|  |_|______| (_) (_) (_)
//
// Need to figure out the best sleep mode based on the peripherals
// that actually need to be clocked rather than just going to the
// least restrictive sleep mode

void mbed_enter_sleep(sleep_t *obj)
{
    (void)obj;
    smc_power_mode_protection_config_t sleep_config = {
        .vlpProt = true,            /*!< VLP protect*/
        .llsProt = true,            /*!< LLS protect */
        .vllsProt = true,           /*!< VLLS protect*/
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
        .hsrunProt = true,          /*!< HSRUN protect */
#endif
    };
    SMC_HAL_SetProtection(SMC_BASE, &sleep_config);

    //Regular sleep for ARM core:
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}

void mbed_exit_sleep(sleep_t *obj)
{
    (void)obj;
}

