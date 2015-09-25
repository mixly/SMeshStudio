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
#include "mbed_assert.h"
#include "i2c_api.h"

#if DEVICE_I2C

#include "cmsis.h"
#include "pinmap.h"
#include "fsl_clock_manager.h"
#include "fsl_i2c_hal.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "PeripheralPins.h"
#include "uvisor-lib/uvisor-lib.h"

// Internal states of I2c object used in the IRQ to retreive the state
#define I2C_MASK_10BIT              4
#define I2C_MASK_SLAVE_DISCOVERY    16

#define I2C_SEND                          0
#define I2C_RECEIVE                       1
#define I2C_SEND_SLAVE_DISCOVERY          (I2C_MASK_SLAVE_DISCOVERY + I2C_SEND)
#define I2C_RECEIVE_SLAVE_DISCOVERY       (I2C_MASK_SLAVE_DISCOVERY + I2C_RECEIVE)
#define I2C_10BIT_SEND_SLAVE_DISCOVERY    (I2C_MASK_10BIT + I2C_MASK_SLAVE_DISCOVERY + I2C_SEND)
#define I2C_10BIT_RECEIVE_SLAVE_DISCOVERY (I2C_MASK_10BIT + I2C_MASK_SLAVE_DISCOVERY + I2C_RECEIVE)

static void i2c_buffer_set(i2c_t *obj, void *tx, size_t tx_length, void *rx, size_t rx_length);
static void i2c_enable_vector_interrupt(i2c_t *obj, uint32_t handler, uint8_t enable);
static void i2c_buffer_write(i2c_t *obj);
static void i2c_buffer_read(i2c_t *obj);
static void i2c_start_write_asynch(i2c_t *obj, uint8_t address, uint8_t is10_bit_add);
static void i2c_start_read_asynch(i2c_t *obj, uint8_t address, uint8_t is10_bit_add);

void i2c_init(i2c_t *obj, PinName sda, PinName scl) {
    uint32_t i2c_sda = pinmap_peripheral(sda, PinMap_I2C_SDA);
    uint32_t i2c_scl = pinmap_peripheral(scl, PinMap_I2C_SCL);
    obj->i2c.instance = pinmap_merge(i2c_sda, i2c_scl);
    MBED_ASSERT((int)obj->i2c.instance != NC);

    CLOCK_SYS_EnableI2cClock(obj->i2c.instance);
    uint32_t i2c_addrs[] = I2C_BASE_ADDRS;
    obj->i2c.base_addrs = i2c_addrs[obj->i2c.instance];
    I2C_HAL_Init(obj->i2c.base_addrs);
    I2C_HAL_Enable(obj->i2c.base_addrs);
    I2C_HAL_SetIntCmd(obj->i2c.base_addrs, true);
    i2c_frequency(obj, 100000);

    pinmap_pinout(sda, PinMap_I2C_SDA);
    pinmap_pinout(scl, PinMap_I2C_SCL);

    uint32_t port_addrs[] = PORT_BASE_ADDRS;
    PORT_HAL_SetOpenDrainCmd(port_addrs[sda >> GPIO_PORT_SHIFT], sda & 0xFF, true);
    PORT_HAL_SetOpenDrainCmd(port_addrs[scl >> GPIO_PORT_SHIFT], scl & 0xFF, true);
}

int i2c_start(i2c_t *obj) {
    I2C_HAL_SendStart(obj->i2c.base_addrs);
    return 0;
}

int i2c_stop(i2c_t *obj) {
    volatile uint32_t n = 0;
    if (I2C_HAL_IsMaster(obj->i2c.base_addrs))
        I2C_HAL_SendStop(obj->i2c.base_addrs);

    // It seems that there are timing problems
    // when there is no waiting time after a STOP.
    // This wait is also included on the samples
    // code provided with the freedom board
    for (n = 0; n < 200; n++) __NOP();
    return 0;
}

static int timeout_status_poll(i2c_t *obj, i2c_status_flag_t flag) {
    uint32_t i, timeout = 100000;

    for (i = 0; i < timeout; i++) {
        if (I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, flag))
            return 0;
    }
    return 1;
}

// this function waits the end of a tx transfer and return the status of the transaction:
//    0: OK ack received
//    1: OK ack not received
//    2: failure
static int i2c_wait_end_tx_transfer(i2c_t *obj) {
    // wait for the interrupt flag

    if (timeout_status_poll(obj, kI2CInterruptPending)) {
        return 2;
    }
    I2C_HAL_ClearInt(obj->i2c.base_addrs);

    // wait transfer complete
    if (timeout_status_poll(obj, kI2CTransferComplete)) {
        return 2;
    }

    // check if we received the ACK or not
    return I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, kI2CReceivedNak) ? 1 : 0;
}

// this function waits the end of a rx transfer and return the status of the transaction:
//    0: OK
//    1: failure
static int i2c_wait_end_rx_transfer(i2c_t *obj) {
    // wait for the end of the rx transfer
    if (timeout_status_poll(obj, kI2CInterruptPending)) {
        return 1;
    }
    I2C_HAL_ClearInt(obj->i2c.base_addrs);

    return 0;
}

static int i2c_do_write(i2c_t *obj, int value) {
    I2C_HAL_WriteByte(obj->i2c.base_addrs, value);

    // init and wait the end of the transfer
    return i2c_wait_end_tx_transfer(obj);
}

static int i2c_do_read(i2c_t *obj, char * data, int last) {
    if (last) {
        I2C_HAL_SendNak(obj->i2c.base_addrs);
    } else {
        I2C_HAL_SendAck(obj->i2c.base_addrs);
    }

    *data = (I2C_HAL_ReadByte(obj->i2c.base_addrs) & 0xFF);

    // start rx transfer and wait the end of the transfer
    return i2c_wait_end_rx_transfer(obj);
}

void i2c_frequency(i2c_t *obj, int hz) {
    uint32_t busClock;
    clock_manager_error_code_t error = CLOCK_SYS_GetFreq(kBusClock, &busClock);
    if (error == kClockManagerSuccess) {
        I2C_HAL_SetBaudRate(obj->i2c.base_addrs, busClock, hz / 1000, NULL);
    }
}

int i2c_read(i2c_t *obj, int address, char *data, int length, int stop) {
    int count;
    char dummy_read, *ptr;

    if (i2c_start(obj)) {
        i2c_stop(obj);
        return I2C_ERROR_BUS_BUSY;
    }

    if (i2c_do_write(obj, (address | 0x01))) {
        i2c_stop(obj);
        return I2C_ERROR_NO_SLAVE;
    }

    // set rx mode
    I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CReceive);

    // Read in bytes
    for (count = 0; count < (length); count++) {
        ptr = (count == 0) ? &dummy_read : &data[count - 1];
        uint8_t stop_ = (count == (length - 1)) ? 1 : 0;
        if (i2c_do_read(obj, ptr, stop_)) {
            i2c_stop(obj);
            return count;
        }
    }

    // If not repeated start, send stop.
    if (stop)
        i2c_stop(obj);

    // last read
    data[count-1] = I2C_HAL_ReadByte(obj->i2c.base_addrs);

    return length;
}

int i2c_write(i2c_t *obj, int address, const char *data, int length, int stop) {
    int i;

    if (i2c_start(obj)) {
        i2c_stop(obj);
        return I2C_ERROR_BUS_BUSY;
    }

    if (i2c_do_write(obj, (address & 0xFE))) {
        i2c_stop(obj);
        return I2C_ERROR_NO_SLAVE;
    }

    for (i = 0; i < length; i++) {
        if(i2c_do_write(obj, data[i])) {
            i2c_stop(obj);
            return i;
        }
    }

    if (stop)
        i2c_stop(obj);

    return length;
}

void i2c_reset(i2c_t *obj) {
    i2c_stop(obj);
}

int i2c_byte_read(i2c_t *obj, int last) {
    char data;
    // set rx mode
    I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CReceive);

    // Setup read
    i2c_do_read(obj, &data, last);

    // set tx mode
    I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CSend);
    return I2C_HAL_ReadByte(obj->i2c.base_addrs);
}

int i2c_byte_write(i2c_t *obj, int data) {
    // set tx mode
    I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CSend);

    return !i2c_do_write(obj, (data & 0xFF));
}

#if DEVICE_I2C_ASYNCH

static void i2c_buffer_write(i2c_t *obj)
{
    if (obj->tx_buff.pos < obj->tx_buff.length) {
        uint8_t *tx = (uint8_t *)obj->tx_buff.buffer;
        I2C_HAL_WriteByte(obj->i2c.base_addrs, tx[obj->tx_buff.pos]);
        obj->tx_buff.pos++;
    }
}

static void i2c_buffer_read(i2c_t *obj)
{
    uint8_t *rx = (uint8_t *)obj->rx_buff.buffer;
    rx[obj->rx_buff.pos] = I2C_HAL_ReadByte(obj->i2c.base_addrs);
    obj->rx_buff.pos++;
}

static void i2c_start_write_asynch(i2c_t *obj, uint8_t address, uint8_t is10_bit_add)
{
    obj->i2c.state = is10_bit_add ? I2C_10BIT_SEND_SLAVE_DISCOVERY : I2C_SEND_SLAVE_DISCOVERY;
    I2C_HAL_SendStart(obj->i2c.base_addrs);
    I2C_HAL_WriteByte(obj->i2c.base_addrs, (address & 0xFE));
}

static void i2c_start_read_asynch(i2c_t *obj, uint8_t address, uint8_t is10_bit_add)
{
    obj->i2c.state = is10_bit_add ? I2C_10BIT_RECEIVE_SLAVE_DISCOVERY : I2C_RECEIVE_SLAVE_DISCOVERY;
    I2C_HAL_SendStart(obj->i2c.base_addrs);
    I2C_HAL_WriteByte(obj->i2c.base_addrs, (address | 0x01));
}

void i2c_transfer_asynch(i2c_t *obj, void *tx, size_t tx_length, void *rx, size_t rx_length, uint32_t address, uint32_t stop, uint32_t handler, uint32_t event, DMAUsage hint)
{
    (void) hint;
    obj->i2c.dma_state = DMA_USAGE_NEVER;
    obj->i2c.generate_stop = stop;
    obj->i2c.address = address;

    obj->i2c.event = event;
    i2c_buffer_set(obj, tx, tx_length, rx, rx_length);

    uint8_t is10_bit_add = 0;
    // modify address to be sent
    if ((obj->i2c.address >> 10) == 0x1E) {
        address = obj->i2c.address >> 8;
        is10_bit_add = 1;
    }

    I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CSend);
    // tx buffer set or rx empty (advertise only) - sending, otherwise receiving
    if ((obj->tx_buff.buffer && obj->tx_buff.length) || (obj->rx_buff.buffer == NULL || obj->rx_buff.length == 0)) {
        i2c_start_write_asynch(obj, address, is10_bit_add);
    } else {
        i2c_start_read_asynch(obj, address, is10_bit_add);
    }
    i2c_enable_vector_interrupt(obj, handler, true);
}

static void i2c_enable_vector_interrupt(i2c_t *obj, uint32_t handler, uint8_t enable)
{
    IRQn_Type i2c_irq[] = I2C_IRQS;
    if (enable) {
        obj->i2c.vector_cur = handler;
        obj->i2c.vector_prev = vIRQ_GetVector(i2c_irq[obj->i2c.instance]);
        vIRQ_SetVector(i2c_irq[obj->i2c.instance], handler);
        vIRQ_EnableIRQ(i2c_irq[obj->i2c.instance]);
    } else {
        vIRQ_SetVector(i2c_irq[obj->i2c.instance], handler);
        vIRQ_DisableIRQ(i2c_irq[obj->i2c.instance]);
    }
}

uint32_t i2c_irq_handler_asynch(i2c_t *obj)
{
    int event = 0;
    I2C_HAL_ClearInt(obj->i2c.base_addrs);

    if (!I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, kI2CBusBusy)) {
        return event;
    }

    if (obj->i2c.state & I2C_MASK_SLAVE_DISCOVERY) {
        if (I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, kI2CReceivedNak)) {
            i2c_abort_asynch(obj);
            if (obj->i2c.event & I2C_EVENT_ERROR_NO_SLAVE) {
                event = I2C_EVENT_ERROR_NO_SLAVE;
            }
            return event;
        }
        // There are 4 states for the slave discovery
        switch (obj->i2c.state) {
            case I2C_10BIT_SEND_SLAVE_DISCOVERY:
                obj->i2c.state = I2C_SEND_SLAVE_DISCOVERY;
                I2C_HAL_WriteByte(obj->i2c.base_addrs, (obj->i2c.address & 0xFF));
                break;
            case I2C_10BIT_RECEIVE_SLAVE_DISCOVERY:
                obj->i2c.state = I2C_RECEIVE_SLAVE_DISCOVERY;
                I2C_HAL_WriteByte(obj->i2c.base_addrs, (obj->i2c.address & 0xFF));
                break;
            case I2C_SEND_SLAVE_DISCOVERY:
                if ((obj->tx_buff.buffer == NULL) || (obj->tx_buff.length == 0)) {
                    // only to send address + ack was received
                    event = I2C_EVENT_TRANSFER_COMPLETE;
                    if (obj->i2c.generate_stop) {
                        i2c_stop(obj);
                    }
                } else {
                    obj->i2c.state = I2C_SEND;
                    i2c_buffer_write(obj);
                }
                break;
            case I2C_RECEIVE_SLAVE_DISCOVERY:
                I2C_HAL_SetDirMode(obj->i2c.base_addrs, kI2CReceive);
                obj->i2c.state = I2C_RECEIVE;
                // if there's 1 byte to read and stop, send NAK, otherwise ACK (the following read - FACK=0)
                if ((obj->rx_buff.length == 1) && (obj->i2c.generate_stop)) {
                    I2C_HAL_SendNak(obj->i2c.base_addrs);
                } else {
                    I2C_HAL_SendAck(obj->i2c.base_addrs);
                }
                I2C_HAL_ReadByte(obj->i2c.base_addrs); //dummy read, as described for i2c kinetis module
                break;
            default:
                break;
        }
    } else {
        // sending/receiving data
        if (obj->i2c.state == I2C_SEND) {
            if (I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, kI2CReceivedNak)) {
                // Something went wrong - slave might have full buffer. Stop the current transfer
                event = I2C_EVENT_TRANSFER_EARLY_NACK;
                if (obj->i2c.generate_stop) {
                    i2c_stop(obj);
                }
            } else {
                uint8_t transfer_compl = I2C_HAL_GetStatusFlag(obj->i2c.base_addrs, kI2CTransferComplete);
                // tx - continue/stop or switch to rx
                if (((obj->tx_buff.pos == obj->tx_buff.length) && transfer_compl)) {
                    event = I2C_EVENT_TRANSFER_COMPLETE;
                    if (obj->rx_buff.buffer && obj->rx_buff.length) {
                        event = 0; //wait for RX to complete, erase the event
                        uint16_t address = obj->i2c.address;
                        uint8_t is10_bit_add = 0;
                        if ((address >> 10) == 0x1E) {
                            address = obj->i2c.address >> 7;
                            is10_bit_add = 1;
                        }
                        i2c_start_read_asynch(obj, address, is10_bit_add);
                    } else {
                        // all send
                        if (obj->i2c.generate_stop) {
                            i2c_stop(obj);
                        }
                    }
                } else if (transfer_compl) {
                    i2c_buffer_write(obj);
                }
            }
        } else if (obj->rx_buff.pos < obj->rx_buff.length) {
            int left = obj->rx_buff.length - obj->rx_buff.pos;
            switch (left) {
                case 0x1:
                    event = I2C_EVENT_TRANSFER_COMPLETE;
                    if (obj->i2c.generate_stop) {
                        i2c_stop(obj);
                    }
                    break;
                case 0x2:
                    // NAK only if there's stop or restart condition (stop-start)
                    if (obj->i2c.generate_stop) {
                        I2C_HAL_SendNak(obj->i2c.base_addrs);
                    } else {
                        I2C_HAL_SendAck(obj->i2c.base_addrs);
                    }
                    break;
                default:
                    I2C_HAL_SendAck(obj->i2c.base_addrs);
                    break;
            }
            i2c_buffer_read(obj);
        }
    }

    if (event) {
        i2c_enable_vector_interrupt(obj, obj->i2c.vector_prev, false);
    }

    return (event & obj->i2c.event);
}

uint8_t i2c_active(i2c_t *obj)
{
    uint8_t result = 0;
    switch (obj->i2c.dma_state) {
        default:
            if ((obj->tx_buff.buffer && (obj->tx_buff.pos < obj->tx_buff.length)) ||
                (obj->rx_buff.buffer && (obj->rx_buff.pos < obj->rx_buff.length))) {
                result = 1;
            }
            break;
    }
    return result;
}

void i2c_abort_asynch(i2c_t *obj)
{
    i2c_enable_vector_interrupt(obj, obj->i2c.vector_prev, false);
    i2c_stop(obj);
}

static void i2c_buffer_set(i2c_t *obj, void *tx, size_t tx_length, void *rx, size_t rx_length)
{
    obj->tx_buff.buffer = tx;
    obj->tx_buff.length = tx_length;
    obj->tx_buff.pos = 0;
    obj->rx_buff.buffer = rx;
    obj->rx_buff.length = rx_length;
    obj->rx_buff.pos = 0;
}

#endif

#endif
