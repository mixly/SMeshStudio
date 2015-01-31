#include "Motor.h"

Motor::Motor(Bricktronics* b,
             uint8_t dir_p,
             uint8_t pwm_p,
             uint8_t en_p,
             uint8_t tach_a_p,
             uint8_t tach_b_p)
{
    brick = b;
    dir_pin = dir_p;
    pwm_pin = pwm_p;
    en_pin = en_p;
    encoder = new Encoder(tach_a_p, tach_b_p);
}

Motor::Motor(Bricktronics* b, uint8_t port)
{
    brick = b;
    set_port(port);
}

void Motor::begin(void)
{
    brick->pinMode(dir_pin, OUTPUT);
    brick->pinMode(pwm_pin, OUTPUT);
    brick->pinMode(en_pin, OUTPUT);
}

void Motor::stop(void)
{
    enabled = false;
    brick->digitalWrite(en_pin, LOW);
    brick->digitalWrite(dir_pin, LOW);
    brick->digitalWrite(pwm_pin, LOW);
}

void Motor::set_port(uint8_t port)
{
    //do this better.
    // TODO We are using new here on the Encoder object - Should we free this next time we call set_port?
    // if (encoder)
    //     free(encoder);
    // or something like that?
    if (port == 1)
    {
        dir_pin = MOTOR_1_DIR;
        pwm_pin = MOTOR_1_PWM;
        en_pin = MOTOR_1_EN;
        encoder = new Encoder(MOTOR_1_TACH_0, MOTOR_1_TACH_1);
    }
    else if (port == 2)
    {
        dir_pin = MOTOR_2_DIR;
        pwm_pin = MOTOR_2_PWM;
        en_pin = MOTOR_2_EN;
        encoder = new Encoder(MOTOR_2_TACH_0, MOTOR_2_TACH_1);
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
        brick->digitalWrite(dir_pin, HIGH);
        analogWrite(pwm_pin, 255+s);
        brick->digitalWrite(en_pin, HIGH);
    }
    else
    {
        brick->digitalWrite(dir_pin, LOW);
        analogWrite(pwm_pin, s);
        brick->digitalWrite(en_pin, HIGH);
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

