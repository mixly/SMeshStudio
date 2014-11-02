#include "BricktronicsMegashield.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

BricktronicsMegashield::BricktronicsMegashield() 
{
}

void BricktronicsMegashield::delay_update(uint16_t delay_ms, PIDMotor* m1, PIDMotor* m2, PIDMotor* m3, PIDMotor* m4, PIDMotor* m5, PIDMotor* m6)
{
    unsigned long end_time = millis() + delay_ms;
    while (millis() < end_time)
    {
        if (m1) m1->update();
        if (m2) m2->update();
        if (m3) m3->update();
        if (m4) m4->update();
        if (m5) m5->update();
        if (m6) m6->update();
        delay(DELAY_UPDATE_MS);
    }
}

void BricktronicsMegashield::begin() {}

