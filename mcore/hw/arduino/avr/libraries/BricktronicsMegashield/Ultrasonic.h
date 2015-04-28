/* BricktronicsMegashield Library -- Ultrasonic.h
   Copyright (C) 2013 Adam Wolf, Matthew Beckler, John Baichtal

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define READ_COMMAND 0x41
#define READ_MEASUREMENT_BYTE_ZERO 0x42

#define GET_VERSION 0x00
#define GET_PRODUCT_ID 0x08
#define GET_SENSOR_TYPE 0x10
#define FACTORY_ZERO 0x11
#define FACTORY_SCALE_FACTOR 0x12
#define FACTORY_SCALE_DEVICOR 0x13
#define UNITS 0x14

#define ULTRASONIC_ADDRESS 0x02
#define BUFF_LEN 9

#include "BricktronicsMegashield.h"

class BricktronicsMegashield;

class Ultrasonic
{
    public:
        Ultrasonic(BricktronicsMegashield* b, uint8_t port);

        BricktronicsMegashield* bms;

        uint8_t sda_pin;
        uint8_t scl_pin;

        uint8_t b_buf[BUFF_LEN];

        SoftI2cMaster i2c;
        void begin(void);

        char* readString(uint8_t, uint8_t);
        uint8_t readBytes(uint8_t, uint8_t, uint8_t*);
        uint8_t readByte(uint8_t);
        bool writeBytes(uint8_t, uint8_t, uint8_t*);
        uint8_t writeByte(uint8_t, uint8_t);

        char* getProductID();
        char* getVersion();
        char* getSensorType();

        uint8_t getDistance(void);

    private:

};

#endif

