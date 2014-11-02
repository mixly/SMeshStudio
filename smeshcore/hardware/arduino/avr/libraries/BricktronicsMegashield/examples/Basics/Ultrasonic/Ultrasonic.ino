#include <Wire.h>
#include <BricktronicsMegashield.h>

// Bricktronics Megashield Example: Ultrasonic
// http://www.wayneandlayne.com/bricktronics/
// This example uses an Ultrasonic Sensor.

// This example reads the Ultrasonic Sensor and reports the distance to an obstruction in front of the sensor in centimeters through the Serial Console.  It has a range of 2.5 meters, resolution of 1 cm, and reported accuracy of +- 3 cm.  255 is sometimes reported as an error state.

// This example uses an Ultrasonic Sensor, so it needs more voltage than a USB port usually gives.  Use an external power supply that provides between 9V and 7.2V.  Two options that work really well are a 9V wall adapter (2.1mm plug, center positive) or a 6xAA battery pack (just like when running motors).

// Connect an Ultrasonic Sensor to Sensor Port 4. Make sure to adjust the jumpers on Sensor Port 4 so that pins 1 and 2 are connected, and pins 4 and 5 are connected.  There's an image of this on the Bricktronics website.

BricktronicsMegashield brick = BricktronicsMegashield();
Ultrasonic ultrasonic = Ultrasonic(&brick, 4); // Plug an Ultrasonic Sensor into Sensor Port 4, and use the jumpers to connect pins 1 and 2, and 4 and 5.

void setup() 
{
    Serial.begin(115200);
    brick.begin();
    ultrasonic.begin();
}

void loop()
{
    Serial.println(ultrasonic.getDistance());
    delay(100);
}

