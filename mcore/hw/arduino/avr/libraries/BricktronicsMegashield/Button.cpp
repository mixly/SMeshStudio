#include "Button.h"

Button::Button(BricktronicsMegashield* b, uint8_t port)
{
    bms = b;

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
    pinMode(input_pin, INPUT_PULLUP);
}

bool Button::is_pressed()
{
    if (digitalRead(input_pin) == HIGH)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Button::is_released()
{
    if (digitalRead(input_pin) == LOW)
    {
        return false;
    }
    else
    {
        return true;
    }
}

