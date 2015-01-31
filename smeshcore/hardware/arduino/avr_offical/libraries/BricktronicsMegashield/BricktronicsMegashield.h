/* Bricktronics Megashield Library
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

#ifndef BRICKTRONICS_MEGASHIELD_H
#define BRICKTRONICS_MEGASHIELD_H

#include <inttypes.h>
#include "utility/Encoder.h"
#include "utility/SoftI2cMaster.h"
#include "utility/digitalWriteFast.h"

#include "ColorSensor.h"
#include "Ultrasonic.h"
#include "Button.h"
#include "Motor.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DELAY_UPDATE_MS 50

// Connections to each of the six motor ports
#define MOTOR_1_EN 55
#define MOTOR_1_DIR 54
#define MOTOR_1_PWM 11
#define MOTOR_1_TACH_0 3
#define MOTOR_1_TACH_1 12

#define MOTOR_2_EN 56
#define MOTOR_2_DIR 57
#define MOTOR_2_PWM 9
#define MOTOR_2_TACH_0 2
#define MOTOR_2_TACH_1 10

#define MOTOR_3_EN 59
#define MOTOR_3_DIR 58
#define MOTOR_3_PWM 8
#define MOTOR_3_TACH_0 18
#define MOTOR_3_TACH_1 7

#define MOTOR_4_EN 61
#define MOTOR_4_DIR 60
#define MOTOR_4_PWM 6
#define MOTOR_4_TACH_0 19
#define MOTOR_4_TACH_1 14

#define MOTOR_5_EN 63
#define MOTOR_5_DIR 62
#define MOTOR_5_PWM 5
#define MOTOR_5_TACH_0 20
#define MOTOR_5_TACH_1 15

#define MOTOR_6_EN 65
#define MOTOR_6_DIR 64
#define MOTOR_6_PWM 4
#define MOTOR_6_TACH_0 21
#define MOTOR_6_TACH_1 16

// Connections to each of the four sensor ports
#define S1_ANA A12
#define S1_DA 25
#define S1_DB 29

#define S2_ANA A13
#define S2_DA 37
#define S2_DB 39

#define S3_ANA A14
#define S3_DA 47
#define S3_DB 49

#define S4_ANA A15
#define S4_DA 51
#define S4_DB 53


class Motor;
class PIDMotor;

class BricktronicsMegashield
{
    public:
        BricktronicsMegashield();
        void begin();
        static void delay_update(uint16_t delay_ms, PIDMotor* m1, PIDMotor* m2, PIDMotor* m3, PIDMotor* m4, PIDMotor* m5, PIDMotor* m6);
    private:
};

#endif

