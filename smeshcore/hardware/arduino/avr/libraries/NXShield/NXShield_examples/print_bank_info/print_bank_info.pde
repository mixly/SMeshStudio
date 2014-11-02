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

#include <Wire.h>
#include <NXShield.h>
#include <ACCLNx.h>
#include <NXTUS.h>


NXShield    nxshield;

int inByte = 0;

void setup()
{
  char            str[80];

  Serial.begin(115200);       // start serial for output
  delay(500);                // wait, allowing time to
                             // activate the serial monitor

  //
  // NXShield supports multiple transports
  // Initialize the transport for i2c devices.
  //
  nxshield.init(SH_SoftwareI2C);

  //
  // Wait until user presses GO button to continue the program
  //
  Serial.println ("Press GO button to continue");
  nxshield.waitForButtonPress(BTN_GO);

}

void
loop()
{
  int ba_v1, ba_v2, bb_v1, bb_v2, bb_us;
  char            str[256];
  int accl_x;
  bool touchPressed;
  bool status;

  while (true) {  

    if (Serial.available() > 0) {
    //
    // get user input from serial window
    //
    inByte = Serial.read();
    sprintf (str, "input: %c", inByte); 
    Serial.println(str);
        
    switch (inByte) {
        case '1':
          // turn on RED LED
          nxshield.ledSetRGB(8, 0, 0);
          break;
        case '2':
          // turn on Green LED
          nxshield.ledSetRGB(0, 8, 0);
          break;
        case '3':
          // turn on blue LED
          nxshield.ledSetRGB(0, 0, 8);
          break;

        default:
          sprintf (str, "this key not implemented");
          Serial.println(str);
          break;
      }
    } else {
      strcpy (str, "BA ... ");
      strcat (str, "F/w Version: ");
      strcat (str, nxshield.bank_a.getFirmwareVersion());
      strcat (str, ", Vendor ID: ");
      strcat (str, nxshield.bank_a.getVendorID());
      strcat (str, ", Device ID: ");
      strcat (str, nxshield.bank_a.getDeviceID());
      strcat (str, ", Feature Set: ");
      strcat (str, nxshield.bank_a.getFeatureSet());
      Serial.println(str);

			delay (100);
      strcpy (str, "BB ... ");
      strcat (str, "F/w Version: ");
      strcat (str, nxshield.bank_b.getFirmwareVersion());
      strcat (str, ", Vendor ID: ");
      strcat (str, nxshield.bank_b.getVendorID());
      strcat (str, ", Device ID: ");
      strcat (str, nxshield.bank_b.getDeviceID());
      strcat (str, ", Feature Set: ");
      strcat (str, nxshield.bank_b.getFeatureSet());
      Serial.println(str);

      int voltage;
      voltage = nxshield.bank_a.nxshieldGetBatteryVoltage();
      Serial.print ("Voltage (milli-volts): ");
      Serial.println ( voltage );

    }
    delay (1000);
  }
}
