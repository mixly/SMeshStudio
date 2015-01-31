/* Bricktronics Shield Library
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

#ifndef BRICKTRONICS_H
#define BRICKTRONICS_H

#include <inttypes.h>
#include "utility/Encoder.h"
#include "utility/Adafruit_MCP23017.h"
#include "utility/SoftI2cMaster.h"
#include "utility/digitalWriteFast.h"

#include "ColorSensor.h"
#include "Ultrasonic.h"
#include "Button.h"
#include "Motor.h"


#include "Arduino.h"


#define DELAY_UPDATE_MS 50

#define MOTOR_1_EN 79
#define MOTOR_1_PWM 10
#define MOTOR_1_DIR 78
#define MOTOR_1_TACH_0 2
#define MOTOR_1_TACH_1 5

#define MOTOR_2_EN 77
#define MOTOR_2_PWM 9
#define MOTOR_2_DIR 76
#define MOTOR_2_TACH_0 3
#define MOTOR_2_TACH_1 4

#define TIP0 11
#define TIP1 71

#define S1_ANA A0
#define S1_DA 70
#define S1_DB 69

#define S2_ANA A1
#define S2_DA 68
#define S2_DB 67

#define S3_ANA A2
#define S3_DA 8
#define S3_DB 12

#define S4_ANA A3
#define S4_DA 7
#define S4_DB 6



class Motor;
class PIDMotor;

class Bricktronics
{
    public:
        Bricktronics();
        void begin();
        Adafruit_MCP23017 mcp;
        void digitalWrite(uint8_t pin, uint8_t level);
        uint8_t digitalRead(uint8_t pin);
        void pinMode(uint8_t pin, uint8_t mode);
        void pullUp(uint8_t pin, uint8_t level);
        static void delay_update(uint16_t delay_ms, PIDMotor* m1, PIDMotor* m2);
    private:
};

#endif

