#include "Motor.h"

Motor::Motor(BricktronicsMegashield* brick,
             uint8_t dir_p,
             uint8_t pwm_p,
             uint8_t en_p,
             uint8_t tach_a_p,
             uint8_t tach_b_p)
{
    bms = brick;
    dir_pin = dir_p;
    pwm_pin = pwm_p;
    en_pin = en_p;
    encoder = new Encoder(tach_a_p, tach_b_p);
}

Motor::Motor(BricktronicsMegashield* brick, uint8_t port)
{
    bms = brick;
    set_port(port);
}

void Motor::begin(void)
{
    pinMode(dir_pin, OUTPUT);
    pinMode(pwm_pin, OUTPUT);
    pinMode(en_pin, OUTPUT);
}

void Motor::stop(void)
{
    enabled = false;
    digitalWrite(en_pin, LOW);
    digitalWrite(dir_pin, LOW);
    digitalWrite(pwm_pin, LOW);
}

void Motor::set_port(uint8_t port)
{
    //do this better - Also what about adding something like this?
    // if (encoder) free(encoder);
    switch (port)
    {
        case 1:
            dir_pin = MOTOR_1_DIR;
            pwm_pin = MOTOR_1_PWM;
            en_pin = MOTOR_1_EN;
            encoder = new Encoder(MOTOR_1_TACH_0, MOTOR_1_TACH_1);
            break;
        case 2:
            dir_pin = MOTOR_2_DIR;
            pwm_pin = MOTOR_2_PWM;
            en_pin = MOTOR_2_EN;
            encoder = new Encoder(MOTOR_2_TACH_0, MOTOR_2_TACH_1);
            break;
        case 3:
            dir_pin = MOTOR_3_DIR;
            pwm_pin = MOTOR_3_PWM;
            en_pin = MOTOR_3_EN;
            encoder = new Encoder(MOTOR_3_TACH_0, MOTOR_3_TACH_1);
            break;
        case 4:
            dir_pin = MOTOR_4_DIR;
            pwm_pin = MOTOR_4_PWM;
            en_pin = MOTOR_4_EN;
            encoder = new Encoder(MOTOR_4_TACH_0, MOTOR_4_TACH_1);
            break;
        case 5:
            dir_pin = MOTOR_5_DIR;
            pwm_pin = MOTOR_5_PWM;
            en_pin = MOTOR_5_EN;
            encoder = new Encoder(MOTOR_5_TACH_0, MOTOR_5_TACH_1);
            break;
        case 6:
            dir_pin = MOTOR_6_DIR;
            pwm_pin = MOTOR_6_PWM;
            en_pin = MOTOR_6_EN;
            encoder = new Encoder(MOTOR_6_TACH_0, MOTOR_6_TACH_1);
            break;
    }
}

void Motor::set_speed(int16_t s)
{
    speed = s;
    if (s == 0)
    {
        stop();
    }
    else if (s < 0)
    {
//Backwards from non-mega...
        digitalWrite(dir_pin, LOW);
        analogWrite(pwm_pin, s*-1);
        digitalWrite(en_pin, HIGH);
    }
    else
    {
        digitalWrite(dir_pin, HIGH);
        analogWrite(pwm_pin, (s*-1)+255);
        digitalWrite(en_pin, HIGH);
    }
}

int16_t Motor::get_speed(void)
{
    return speed;
}

int32_t Motor::get_pos(void)
{
    return encoder->read();
}

void Motor::set_pos(int32_t pos)
{
    encoder->write(pos);
}

void PIDMotor::go_to_pos(int16_t pos)
{
    // clear out errors
    last_error = 0;
    sum_error = 0;
    destination_pos = pos;
}

void PIDMotor::update(void)
{
    int16_t curr_position = encoder->read();
    int32_t error = curr_position - destination_pos;
    int32_t speed = mKP * error;
    speed += mKD * (error - last_error);
    speed += mKI * sum_error;
    last_error = error;
    sum_error += error;
    sum_error = constrain(sum_error, -255, 255);
    speed = constrain(speed, -255, 255);
    set_speed(speed);
}

