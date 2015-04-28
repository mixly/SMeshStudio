/************************************************************************/
/*                                                                 
  Copyright (c) 2011 by mindsensors.com                                
  Email: info (<at>) mindsensors (<dot>) com                          
                                                                    
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

************************************************************************/
/*
For beginners tutorial (and user guide) visit:
http://www.openelectrons.com/docs/viewdoc/1
*/

#include <Wire.h>
#include <NXShield.h>
#include <NXTTouch.h>
#include <NXTLight.h>
#include <SumoEyes.h>
#include <ACCLNx.h>
#include <NXTUS.h>


//
// declare the NXShield(s) attached to your Arduino.
//
NXShield    nxshield;

//
// declare the i2c devices used on NXShield(s).
//

//
// declare analog devices attached to nxshields.
//
SumoEyes    seyes1;

int inByte = 0;

void setup()
{
  char            str[80];

  Serial.begin(115200);       // start serial for output
  delay(500);                // wait, allowing time to
                             // activate the serial monitor
  Serial.println ("Initializing the devices ...");
  //
  // NXShield provides multiple protocols
  // Initialize the protocol for NXShield
  // It is best to use Hardware I2C (unless you want to use Ultrasonic).
  //
  nxshield.init( SH_HardwareI2C );

  //
  // Wait until user presses GO button to continue the program
  //
  Serial.println ("Press GO button to continue");
  nxshield.waitForButtonPress(BTN_GO);

  //
  // Initialize the i2c sensors.
  //

  // initialize the analog sensors.
  seyes1.init( &nxshield, SH_BBS1 );

  Serial.println("\nEnter a character and press 'Send' to begin");
  //
  // wait until user enters a key on serial window.
  //
  while (Serial.available() <= 0) {
    delay(1000);
  }
}

void
loop()
{
  int seyesValue;
  SE_Zone obzone;
  char            str[256];
  char aa[80];
  int accl_x;
  bool touchPressed;
  bool status;
  bool zone = true;

  while (true) {  

    if (Serial.available() > 0) {
    //
    // get user input from serial window
    //
    inByte = Serial.read();
    sprintf (str, "input: %c", inByte); 
    Serial.println(str);
        
    switch (inByte) {
        // if you want to implement any key input processing.
        // do it here.
      
        default:
          break;
      }
    } else {


      Serial.println("----------------------------------------------");
      zone = !zone;
      if ( zone ) {
        status = seyes1.setLongRange();
        delay (500);
      } else {
        status = seyes1.setShortRange();
        delay (500);
      }
      seyesValue = seyes1.readRaw();
      obzone = seyes1.detectObstacleZone();

      sprintf (str, "Sumo Eyes (Range: %s) raw value: %d, obzone: %d (%s)",
                                          zone?"Long":"Short",
                                          seyesValue, obzone,
                                          seyes1.OBZoneToString(obzone) );
      Serial.println(str);

      Serial.println("----------------------------------------------");

    }
    delay (1500);
  }
}

