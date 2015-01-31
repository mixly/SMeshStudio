/* BricktronicsMegashield Library -- ColorSensor.h
Copyright (C) 2013 Adam Wolf, Matthew Beckler, John Baichtal

The contents of this file are subject to the Mozilla Public License Version 1.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.mozilla.org/MPL/
Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the specific language governing rights and limitations under the License.

The Original Code is from leJos.

The Initial Developer of the Original Code is leJos. All Rights Reserved.

Contributor(s): Adam Wolf, Matthew Beckler, John Baichtal.''

Most of the stuff here is original to Wayne and Layne, but one or two functions are straight from leJos, an open source alternative firmware for the NXT.

*/

#ifndef COLORSENSOR_H
#define COLORSENSOR_H

#define COLOR_BLACK 1
#define COLOR_BLUE 2
#define COLOR_GREEN 3
#define COLOR_YELLOW 4
#define COLOR_RED 5
#define COLOR_WHITE 6

#define INDEX_RED 0
#define INDEX_GREEN 1
#define INDEX_BLUE 2
#define INDEX_BLANK 3

#define TYPE_COLORFULL 13
#define TYPE_COLORRED 14
#define TYPE_COLORGREEN 15
#define TYPE_COLORBLUE 16
#define TYPE_COLORNONE 17

#define ADVOLTS 3300
#define ADMAX 1023
#define SENSORMAX ADMAX
#define MINBLANKVAL (214 / (ADVOLTS / ADMAX))

#define CAL_COLUMNS 4
#define CAL_ROWS 3
#define CAL_LIMITS 2

#include "BricktronicsMegashield.h"

class BricktronicsMegashield;

class ColorSensor
{
    public:
        ColorSensor(BricktronicsMegashield* b, uint8_t port);

        BricktronicsMegashield* bms;

        unsigned long calData[CAL_ROWS][CAL_COLUMNS];

        long calLimits[CAL_LIMITS];

        uint16_t raw_values[4];
        uint16_t cal_values[4];

        uint16_t mode;
        uint16_t type;

        uint8_t _data;
        uint8_t _clock;

        void set_clock(int val);
        void set_data(int val);
        bool get_data();
        int read_data();
        void send_mode(unsigned int mode);
        char read_byte_new();
        char read_byte();
        unsigned int calc_CRC(unsigned int crc, unsigned int val);
        bool read_calibration();
        void print_calibration(void);
        bool check_sensor();
        bool check_present();
        int read_full_color_value(int newClock);
        void read_sensor();
        int read_raw_value();
        uint8_t cal_to_color();

        void begin(void);
        void begin(uint8_t);
        void reset_sensor();
        void update();
        int calibrate();
        uint8_t get_color();
        void print_color(uint8_t);
        void set_lamp(uint8_t);

    private:

};

#endif

