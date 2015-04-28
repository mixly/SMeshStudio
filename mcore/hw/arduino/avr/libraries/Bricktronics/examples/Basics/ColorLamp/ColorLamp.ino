#include <Wire.h>
#include <Bricktronics.h>

//Bricktronics Example: ColorLamp
//http://www.wayneandlayne.com/bricktronics
//This example uses a color sensor.

//The Color Sensor can also act like a red, green, or blue lamp.  This example switches between them.

//Connect a Color Sensor into Sensor Port 3.  Make sure to adjust the jumpers on Sensor Port 3 so that only pins 3 and 4 are connected.  There's an image of this on the Bricktronics website.

Bricktronics brick = Bricktronics();
ColorSensor color = ColorSensor(&brick, 3); //Plug a Color Sensor into Sensor Port 3, and adjust the jumpers so that only pins 3 and 4 are connected.

void setup()
{
    Serial.begin(115200);
    brick.begin();
    color.begin();
}

void loop()
{
    color.begin(TYPE_COLORRED);
    delay(500);
    color.begin(TYPE_COLORBLUE);
    delay(500);
    color.begin(TYPE_COLORGREEN);
    delay(500);
    color.begin(TYPE_COLORNONE);
    delay(500);
}
