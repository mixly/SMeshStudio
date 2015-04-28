/* BricktronicsMegashield Library -- Motor.h
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

#ifndef MOTOR_H
#define MOTOR_H

#include "BricktronicsMegashield.h"

#define PID_MOTOR_KP 0.8
#define PID_MOTOR_KD 1.0
#define PID_MOTOR_KI 0.003

class BricktronicsMegashield;

class Motor
{
    public:
        Motor(BricktronicsMegashield* brick,
            uint8_t dir_pin,
            uint8_t pwm_pin,
            uint8_t en_pin,
            uint8_t tach_a_pin,
            uint8_t tach_b_pin);

        Motor(BricktronicsMegashield* brick, uint8_t port);

        Motor(BricktronicsMegashield* brick);

        uint8_t dir_pin;
        uint8_t pwm_pin;
        uint8_t en_pin;
        uint8_t tach_a_pin;
        uint8_t tach_b_pin;

        BricktronicsMegashield* bms;
        Encoder *encoder;

        bool enabled;
        bool direction;
        uint16_t speed;

        void begin(void);
        void stop(void);
        void set_speed(int16_t);
        void set_port(uint8_t);
        int16_t get_speed(void);

        int32_t get_pos(void);
        void set_pos(int32_t);

    private:
};

class PIDMotor: public Motor
{
    public:
        PIDMotor(BricktronicsMegashield* brick,
                uint8_t dir_pin,
                uint8_t pwm_pin,
                uint8_t en_pin,
                uint8_t tach_a_pin,
                uint8_t tach_b_pin) : Motor(brick,
                                            dir_pin,
                                            en_pin,
                                            pwm_pin,
                                            tach_a_pin,
                                            tach_b_pin)
        {
            mKP = PID_MOTOR_KP;
            mKD = PID_MOTOR_KD;
            mKI = PID_MOTOR_KI;
        };


        PIDMotor(BricktronicsMegashield* brick, uint8_t port) : Motor(brick, port)
        {
            mKP = PID_MOTOR_KP;
            mKD = PID_MOTOR_KD;
            mKI = PID_MOTOR_KI;
        };

        PIDMotor(BricktronicsMegashield* brick);

        void go_to_pos(int16_t);
        void update(void);

        int32_t last_error;
        int32_t sum_error;
        int16_t destination_pos;

        float mKP;
        float mKD;
        float mKI;

    private:
};

#endif

