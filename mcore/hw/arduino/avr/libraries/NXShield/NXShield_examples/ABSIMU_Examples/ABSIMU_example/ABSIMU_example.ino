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
#include <AbsoluteIMU.h>

// setup for this example:
// attach external power to NXShield.
// attach AbsoluteIMU to Port BAS1
// Open the Serial terminal to view.

//
// declare the NXShield(s) attached to your Arduino.
//
NXShield    nxshield;

//
// declare the i2c devices used on NXShield(s).
//

AbsoluteIMU imu (0x22);

void setup()
{
  char str[256];
/**  Start serial for output
 */
  Serial.begin(115200);  // 
/**  Wait, allowing time to activate the serial monitor
 */ 
  delay(500); // wait, allowing time to activate the serial monitor


/**  Initialize the protocol for NXShield
     It is best to use Hardware I2C (unless you want to use Ultrasonic).
 */
  nxshield.init( SH_SoftwareI2C );
/**  Wait until user presses GO button to continue the program
 */
  Serial.println ("Press GO button to continue");
  nxshield.waitForButtonPress(BTN_GO);
/**  Initialize the i2c sensors.
 */
  imu.init( &nxshield, SH_BAS1 );

}

#define DELAY_T 0

void loop()
{
  char aa[80];        
  char str[256];          //sets length of character string
  uint8_t  result;
  char str2[20];          //sets length of character string 2
  char str3[20];          //sets length of character string 3 
  gyro mygyro;            //declares mygyro variable
  cmps mycmps;            //declares mycmps variable
  accl myaccl;            //declares myaccl variable
  magnetic_field mymgnt;  //declares mymgnt variable
/**  Displays Firmeware Version of sensor
*/
  strcpy(aa, imu.getFirmwareVersion() );
  sprintf (str, "imu: getFirmwareVersion: %s", aa);
  Serial.println(str);
/**  Displays Device ID of sensor
 */
  strcpy(aa, imu.getDeviceID() );
  sprintf (str, "imu: DeviceID: %s", aa);
  Serial.println(str);
/**  Gets and displays Vendor ID of sensor
 */
  strcpy(aa, imu.getVendorID() );
  sprintf (str, "imu: VendorID: %s", aa);
  Serial.println(str);
/**  Displays Magnetometer reading
 */
  imu.readGyro(mygyro);
  sprintf (str, "imu: gyro      x:%d | y:%d | z:%d", mygyro.gx, mygyro.gy, mygyro.gz);
  Serial.println(str);
/**  Displays Tilt reading
 */  
  imu.readAccelerometer(myaccl);
  sprintf (str, "imu: tilt      x:%d | y:%d | z:%d", myaccl.tx, myaccl.ty, myaccl.tz);
  Serial.println(str);
/**  Displays Accelerometer reading
 */  
  sprintf (str, "imu: accl      x:%d | y:%d | z:%d", myaccl.ax, myaccl.ay, myaccl.az);
  Serial.println(str);
/**  Displays Magnetic Field reading
 */
  imu.readMagneticField(mymgnt);
  sprintf (str, "imu: magfield  x:%d | y:%d | z:%d", mymgnt.mx, mymgnt.my, mymgnt.mz);
  Serial.println(str);
/**  Displays Compass reading
 */
  imu.readCompass(mycmps);
  sprintf (str, "imu: compass:  %d", mycmps.heading );
  Serial.println(str);
  sprintf (str, "imu: error:  %d", mycmps.error );
  Serial.println(str);
 
  Serial.println( "-------------" );
  delay (1500);
}


