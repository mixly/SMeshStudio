#include <Wire.h>
#include <Bricktronics.h>

// Make: LEGO and Arduino Projects
// Chapter 1: Drawbot
// Website: http://www.wayneandlayne.com/bricktronics/

//This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

long timeout = 0;
Bricktronics brick = Bricktronics();
Motor r = Motor(&brick, 1);
Motor l = Motor(&brick, 2);
Button front = Button(&brick, 1);

void setup()
{
    randomSeed(analogRead(A3));
    brick.begin();
    r.begin();
    l.begin();
    front.begin();
}

void bumpers_hit()
{
    int r_speed = r.get_speed() * -0.9;
    int l_speed = l.get_speed() * -0.9;

    r.set_speed(r_speed);
    l.set_speed(l_speed);
    delay(500);

    if (abs(l_speed) < 25 && abs(r_speed) < 25)
    {
        timeout_expired();
    }
}

void timeout_expired()
{
    r.set_speed(random(-127, 127));
    l.set_speed(random(-127, 127));

    timeout = millis() + random(1, 10) * 1000;
}

void loop()
{
    if (timeout != 0 && timeout < millis())
    {
        timeout_expired();
    }

    if (front.is_pressed())
    {
        bumpers_hit();
    }
}
