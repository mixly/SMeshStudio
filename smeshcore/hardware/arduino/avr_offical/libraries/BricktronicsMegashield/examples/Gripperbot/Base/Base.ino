#include <Wire.h>
#include <BricktronicsMegashield.h>

// Getting started with Lego and Arduino
// Chapter 7: GripperBot: Tank Base
// Website: http://www.wayneandlayne.com/bricktronics

// This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

BricktronicsMegashield brick = BricktronicsMegashield();
Motor r = Motor(&brick, 1);
Motor l = Motor(&brick, 2);

void setup()
{
    Serial.begin(9600);
    brick.begin();
    r.begin();
    l.begin();
}

void process_incoming_command(char cmd, char arg0)
{
    int speed = 0;
    switch (cmd)
    {
    case '6':
    case 6:
        speed = arg0*2;
        l.set_speed(speed);
        break;
    case '7':
    case 7:
        speed = arg0*2;
        r.set_speed(speed);
        break;
    default:
        break;
    }
}

void loop() {
    while (Serial.available() < 3)
    {
        //Do nothing while we wait for a full command to come in.
    }
    char start = Serial.read();
    if (start != '*')
    {
        return;
    }

    char cmd = Serial.read();
    char arg0 = Serial.read();
    process_incoming_command(cmd, arg0);
}

