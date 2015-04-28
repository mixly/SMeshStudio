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
#include <LineLeader.h>

// setup for this example:
// attach external power to NXShield.
// attach NXTLineLeader to Port BAS1
// Open the Serial terminal to view.

//
// declare the NXShield(s) attached to your Arduino.
//
NXShield    nxshield;

//
// declare the i2c devices used on NXShield(s).
//
LineLeader ll (0x02);

void setup()
{
  char str[256];

  Serial.begin(115200);  // start serial for output
  delay(500); // wait, allowing time to activate the serial monitor

//  Serial.println (__FILE__);
//  Serial.println ("Initializing the devices ...");
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
  ll.init( &nxshield, SH_BAS1 );

}

#define DELAY_T 0

void loop()
{
  char aa[80];
  char str[256];
  uint8_t  result;
  char str2[20];

  strcpy(aa, ll.getFirmwareVersion() );
  sprintf (str, "ll: getFirmwareVersion: %s", aa);
  Serial.println(str);

  strcpy(aa, ll.getDeviceID() );
  sprintf (str, "ll: DeviceID: %s", aa);
  Serial.println(str);

  strcpy(aa, ll.getVendorID() );
  sprintf (str, "ll: VendorID: %s", aa);
  Serial.println(str);

  result = ll.getResult();
  format_bin(result, str2);
  sprintf (str, "ll: sensor array: %s", str2 );
  Serial.println(str);

  Serial.println( "-------------" );
  delay (1500);
}


