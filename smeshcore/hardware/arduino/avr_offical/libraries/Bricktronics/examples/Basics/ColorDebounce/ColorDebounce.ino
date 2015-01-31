#include <Wire.h>
#include <Bricktronics.h>

//Bricktronics Example: ColorDebounce
//http://www.wayneandlayne.com/bricktronics
//This example uses a color sensor.

//Color sensor readings are taken repeatedly, and when they change, they're debounced so the colors don't switch back and forth on a transition.  This isn't the necessarily the simplest or best way to debounce, but it works for what we're using it for!

//Connect a Color Sensor into Sensor Port 3.  Make sure to adjust the jumpers on Sensor Port 3 so that only pins 3 and 4 are connected.  There's an image of this on the Bricktronics website.

Bricktronics brick = Bricktronics();
ColorSensor color = ColorSensor(&brick, 3); //Plug a Color Sensor into Sensor Port 3, and adjust the jumpers so that only pins 3 and 4 are connected.
void setup()
{
    Serial.begin(115200);
    brick.begin();
    color.begin();
}

char last_color = color.get_color();

void loop()
{
    char stable = 0;
    char new_reading = color.get_color();
    if (new_reading != last_color)
    {
        while (stable < 10)
        {
            delay(10);
            if (color.get_color() == new_reading)
            {
                stable += 1;
            } else
            {
                new_reading = color.get_color();
                stable = 0;
            }
        }

        if (new_reading != last_color)
        {
            color.print_color(new_reading);
            Serial.println();
            last_color = new_reading;
        }
    }
    delay(10);
}
