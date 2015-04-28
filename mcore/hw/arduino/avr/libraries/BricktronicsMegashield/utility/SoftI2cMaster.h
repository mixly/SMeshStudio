/* Arduino SoftI2cMaster Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino SoftI2cMaster Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SoftI2cMaster Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SOFT_I2C_MASTER
#define SOFT_I2C_MASTER

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// R/W direction bit to OR with address for start or restart
#define I2C_READ 1
#define I2C_WRITE 0

// delay used to tweek signals
#define I2C_DELAY_USEC 30

class SoftI2cMaster {
  uint8_t sclPin_;
  uint8_t sdaPin_;
public:
  /** init bus */
  void init(uint8_t sclPin, uint8_t sdaPin);
  
  /** read a byte and send Ack if last is false else Nak to terminate read */
  uint8_t read(uint8_t last);
  
  /** send new address and read/write bit without stop */
  uint8_t restart(uint8_t addressRW);
  
  /** issue a start condition for i2c address with read/write bit */
  uint8_t start(uint8_t addressRW);
  
  /** issue a stop condition */
  void stop(void);
  
  /** write byte and return true for Ack or false for Nak */
  uint8_t write(uint8_t b);
};
#endif //SOFT_I2C_MASTER
