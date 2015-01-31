#include <Wire.h>
#include <BricktronicsMegashield.h>

// Bricktronics Megashield Example: ColorButton
// http://www.wayneandlayne.com/bricktronics/
// This example uses a button and a color sensor.
// When the button is pressed, a single reading from the color sensor is taken and converted into a color name and printed over the Serial Console.

// Connect a button to Sensor Port 1, and a Color Sensor into Sensor Port 3.  Make sure to adjust the jumpers on Sensor Port 3 so that only pins 3 and 4 are connected.  There's an image of this on the Bricktronics website.


BricktronicsMegashield brick = BricktronicsMegashield();
ColorSensor color = ColorSensor(&brick, 3); // Plug a Color Sensor into Sensor Port 3, and adjust the jumpers so that only pins 3 and 4 are connected.
Button button = Button(&brick, 1);

void setup() 
{
    Serial.begin(115200);
    brick.begin();
    button.begin();
    color.begin();
}

void loop()
{
    while (button.is_released())
    {
        // do nothing
    }
    color.print_color(color.get_color());
    Serial.println();
    delay(100); // simple debounce

    while (button.is_pressed())
    {
        // do nothing
    }
    delay(100); // simple debounce
}

