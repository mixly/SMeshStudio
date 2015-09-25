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
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"
#include "dma_api.h"
#include "target_config.h"
#include "fsl_mcg_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_irq_s {
    uint32_t port;
    uint32_t pin;
    uint32_t ch;
};

struct port_s {
    PortName port;
    uint32_t mask;
};

struct pwmout_s {
    PWMName pwm_name;
};

struct serial_s {
    uint32_t instance;
    uint32_t address;
    DMAUsage tx_dma_state;
    DMAUsage rx_dma_state;
    uint8_t databits;
    uint32_t event;
    uint32_t vector_cur;
    uint32_t vector_prev;
    IRQn_Type irq_number;
    uint8_t entry_count;
};

struct analogin_s {
    ADCName adc;
};

struct i2c_s {
    uint32_t instance;
    uint32_t base_addrs;
    uint32_t event;
    uint32_t vector_cur;
    uint32_t vector_prev;
    uint16_t address;
    uint8_t dma_state;
    uint8_t generate_stop;
    uint8_t state;
};

struct spi_s {
    uint32_t instance;
    uint32_t address;
    DMAUsage dma_state;
    uint8_t bits;
    uint32_t event;
};

struct dac_s {
    DACName dac;
};

struct sleep_s {
    mcg_clock_select_t mcg_clock;
};

#include "gpio_object.h"

#ifdef __cplusplus
}
#endif

#endif
