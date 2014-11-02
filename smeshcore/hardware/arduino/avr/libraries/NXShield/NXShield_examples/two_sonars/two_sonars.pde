
/*
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
*/
/*
For beginners tutorial (and user guide) visit:
http://www.openelectrons.com/docs/viewdoc/1
*/

#include <Wire.h>
#include <NXShield.h>
#include <NXTUS.h>

// setup for this example:
// attach external power to NXShield.
// attach LEGO Ultrasonic sensors to BAS2 and BBS2
// Open the Serial terminal to view.


//
// declare the NXShield(s) attached to your Arduino.
//
NXShield    nxshield;

//
// declare the i2c devices used on NXShield(s).
//
NXTUS       sonar1;
NXTUS       sonar2;

void setup()
{
  char str[256];

  Serial.begin(115200);  // start serial for output
  delay(500); // wait, allowing time to activate the serial monitor

  Serial.println (__FILE__);
  Serial.println ("Initializing the devices ...");
  //
  // Initialize the protocol for NXShield
  // It is best to use Hardware I2C (unless you want to use Ultrasonic).
  //
  nxshield.init( SH_SoftwareI2C );

  //
  // Wait until user presses GO button to continue the program
  //
  Serial.println ("Press GO button to continue");
  nxshield.waitForButtonPress(BTN_GO);

  //
  // Initialize the i2c sensors.
  //
  sonar1.init( &nxshield, SH_BBS1 );
  sonar2.init( &nxshield, SH_BAS1 );

}

void loop()
{
  char aa[80];
  char str[256];
  int  bb_us;

  strcpy(aa, sonar1.getFirmwareVersion() );
  sprintf (str, "sonar1: FirmwareVersion: %s", aa);
  Serial.println(str);

  strcpy(aa, sonar1.getDeviceID() );
  sprintf (str, "sonar1: DeviceID: %s", aa);
  Serial.println(str);

  strcpy(aa, sonar1.getVendorID() );
  sprintf (str, "sonar1: VendorID: %s", aa);
  Serial.println(str);

  bb_us = sonar1.getDist();
  sprintf (str, "sonar1: Obstacle at: %d mm", bb_us );
  Serial.println(str);

  strcpy(aa, sonar2.getFirmwareVersion() );
  sprintf (str, "sonar2: FirmwareVersion: %s", aa);
  Serial.println(str);
  strcpy(aa, sonar2.getDeviceID() );
  sprintf (str, "sonar2: DeviceID: %s", aa);
  Serial.println(str);
  strcpy(aa, sonar2.getVendorID() );
  sprintf (str, "sonar2: VendorID: %s", aa);
  Serial.println(str);
  bb_us = sonar2.getDist();
  sprintf (str, "sonar2: Obstacle at: %d mm", bb_us );
  Serial.println(str);

  Serial.println( "-------------" );
  delay (1500);
}


