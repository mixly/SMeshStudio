/************************************************************************/
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

************************************************************************/
/*
For beginners tutorial (and user guide) visit:
http://www.openelectrons.com/docs/viewdoc/1
*/

#include <Wire.h>
#include <NXShield.h>
#include <NXTUS.h>
#include <PSPNx.h>

NXShield          nxshield;
byte ra, rb;
uint8_t motor_status;
uint8_t stalled;
NXTUS       sonar;
PSPNx       pspnx;

void
setup()
{
    Serial.begin(115200);       // start serial for output
    delay(100);                // wait allowing time to
                                // activate the serial monitor

    long            rotations = 1;  // Define variable rotations and set
                                    // equal to 90
    char            str[40];
    stalled = 0;

    nxshield.init( SH_HardwareI2C );

    uint8_t v;

    v = nxshield.bank_a.readByte(SH_VOLTAGE);
    sprintf (str, "voltage byte: %d", v);
    Serial.println ( str );

    int voltage;
    voltage = nxshield.bank_a.nxshieldGetBatteryVoltage();
    Serial.print ("Voltage (milli-volts): ");
    Serial.println ( voltage );

    //
    // reset motors.
    //
    nxshield.bank_a.motorReset();
    nxshield.bank_b.motorReset();

    //
    // Initialize the i2c sensors.
    //
    sonar.init( &nxshield, SH_BBS1 );
    pspnx.init( &nxshield, SH_BBS2 );

    //
    // Wait until user presses GO button to continue the program
    //
    Serial.println ("Press GO button to continue");
    nxshield.waitForButtonPress(BTN_GO);

		/*
    Serial.println("\nEnter a character and press 'Send' to begin");
    //
    // wait until user enters a key on serial window.
    //
    while (Serial.available() <= 0) {
      delay(1000);
    }
		*/
}

void
loop()                          // After the setup the LED blinks to show
                                // the program is complete
{
  int8_t yl, xl, yr, xr, button_set1[8], button_set2[8];
	static uint8_t r, g, b;
  char str[50];
  xl = pspnx.getXLJoy();
  yl = pspnx.getYLJoy();
  yr = pspnx.getYRJoy();
  xr = pspnx.getXRJoy();
  sprintf (str, "xl: %d, yl: %d, xr: %d, yr: %d", xl, yl, xr, yr);
  Serial.println(str);
  if ( yl < 10 && yl > -10 ) {
    // stop the motors if the jostick values are very small
    nxshield.bank_a.motorStop(SH_Motor_2, SH_Next_Action_Brake);
  } else {
    nxshield.bank_a.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, yl);
  }
  if ( yr < 10 && yr > -10 ) {
    // stop the motors if the jostick values are very small
    nxshield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Brake);
  } else {
    nxshield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, yr);
  }
	pspnx.getButtons(button_set1, button_set2);
	// blink the LED in different colors
	// when you press L1/L2/R1/R2 buttons
	if ( button_set2[BS2_Button_R2] == 0 ) {
		delay (200);
	  if ( r != 0 ) r = 0;
		else r = 8;
	}
	if ( button_set2[BS2_Button_R1] == 0 ) {
		delay (200);
	  if ( g != 0 ) g = 0;
		else g = 8;
	}
	if ( button_set2[BS2_Button_L2] == 0 ) {
		delay (200);
	  if ( b != 0 ) b = 0;
		else b = 8;
	}
	if ( button_set2[BS2_Button_L1] == 0 ) {
	  delay (200);
		if ( r != 0 || g != 0 || b != 0 ) {
			r = g = b = 0;
		} else {
			r = g = b = 8;
		}
	}
	nxshield.ledSetRGB(r, g, b);
  delay (100);
}



