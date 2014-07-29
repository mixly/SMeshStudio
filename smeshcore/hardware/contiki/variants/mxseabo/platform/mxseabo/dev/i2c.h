/*
 * Copyright (c) 2011-2013 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * i2c.h
 *
 *  Created on: Nov 27, 2013
 *      Author: smeshlink
 */

#ifndef __I2C_H__
#define __I2C_H__

#define I2C_NONE    0x00
#define I2C_UART    0x01
#define I2C_GPRS    0x02
#define I2C_BUZZER  0x40
#define I2C_5V      0x60

void i2c_init();
void real_init_i2c();
void i2c_config(unsigned char value);
void i2c_delay_us();

#endif /* __I2C_H__ */
