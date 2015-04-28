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
#include <NXTTouch.h>
#include <NXTLight.h>
#include <SumoEyes.h>
#include <ACCLNx.h>
#include <NXTUS.h>


// setup for this example:
// attach external power to NXShield.
// attach a touch sensor to BBS1
// attach motors to motor ports (any number of motors is fine).
// Open the Serial terminal to view.

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
NXTTouch    touch1;

void setup()
{
  char str[256];

  Serial.begin(115200);  // start serial for output
  delay(500); // wait, allowing time to activate the serial monitor

  Serial.println ("Initializing the devices ...");
  //
  // NXShield provides multiple protocols
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

  // initialize the analog sensors.
  touch1.init( &nxshield, SH_BBS1 );

  //
  // reset motors.
  //
  nxshield.bank_a.motorReset();
  nxshield.bank_b.motorReset();

  Serial.println("\nEnter a character and press 'Send' to begin");
  //
  // wait until user enters a key on serial window.
  //
  while (Serial.available() <= 0) {
    delay(1000);
  }

}


void loop()
{
  char aa[80];
  int a;
  char str[256];
  bool touch_status;
  bool last_status;

  while (true) {  

    //
    //  run the motor if touch sensor is pressed.
    //


    int ba_v1, ba_v2, bb_v1, bb_v2, bb_us;
    a = touch1.readRaw();
    ba_v1 = nxshield.bank_a.sensorReadRaw(SH_S1);
    ba_v2 = nxshield.bank_a.sensorReadRaw(SH_S2);
    bb_v1 = nxshield.bank_b.sensorReadRaw(SH_S1);
    bb_v2 = nxshield.bank_b.sensorReadRaw(SH_S2);

    Serial.println("----------------------------------------------");
    sprintf (str, "Raw values bank A, S1: (a: %d), %d, S2: %d", a, ba_v1, ba_v2);
    Serial.println(str);
    sprintf (str, "           bank B, S1: %d S2: %d", bb_v1, bb_v2);
    Serial.println(str);
    Serial.println("----------------------------------------------");

    touch_status = touch1.isPressed();
		sprintf (str, "touch1: is pressed : %s", touch_status?"true":"false");
		Serial.println(str);
		//if ( 1 ) {
    if ( touch_status != last_status ) {
      if ( touch_status ) {
        Serial.println( "run unlimited" );
        nxshield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 50);
        nxshield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 50);
        nxshield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 50);
        nxshield.bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 50);
      } else {
        Serial.println( "stop (float)");
        nxshield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
        nxshield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Float);
        nxshield.bank_a.motorStop(SH_Motor_1, SH_Next_Action_Float);
        nxshield.bank_a.motorStop(SH_Motor_2, SH_Next_Action_Float);
      }
		  last_status = touch_status;
    }

    delay (500);
  }
}


