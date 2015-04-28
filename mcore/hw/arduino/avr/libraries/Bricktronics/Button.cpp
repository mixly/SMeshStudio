#include "Button.h"

Button::Button(Bricktronics* b,	uint8_t port)
{
    brick = b;

    //TODO: do this a better way
    switch (port)
    {
    case 1:
        input_pin = S1_ANA;
        break;
    case 2:
        input_pin = S2_ANA;
        break;
    case 3:
        input_pin = S3_ANA;
        break;
    case 4:
        input_pin = S4_ANA;
        break;
    }
}

void Button::begin(void)
{
    brick->pinMode(input_pin, INPUT);
    brick->pullUp(input_pin, HIGH);
}

bool Button::is_pressed()
{
    if (brick->digitalRead(input_pin) == HIGH)
    {
        return false;
    } else {
        return true;
    }
}

bool Button::is_released()
{
    if (brick->digitalRead(input_pin) == LOW)
    {
        return false;
    } else {
        return true;
    }
}

