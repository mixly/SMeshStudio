#include <Wire.h>
#include <NXShield.h>
#include <NXTCurrentMeter.h>

// setup for this example:
// attach external power to NXShield.
// attach NXTVoltMeter to Port BAS1
// Open the Serial terminal to view.

//
// declare the NXShield(s) attached to your Arduino.
//
NXShield    nxshield;

//
// declare the i2c devices used on NXShield(s).
//

CurrentMeter  im  (0x28);

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
  im.init( &nxshield, SH_BAS1 );
/**  Sets the Reference Current to Absolute Current
 */
  im.setReferenceI();

}


#define DELAY_T 0

void loop()
{
  char  str[256];    //declares character string length
  int  acurr;        //declares "acurr" variable
  int  rcurr;        //declares "rcurr" variable
  int  refI;         //declares "refI" variable
  
/**  Displays Absolute Current value
 */
  acurr = im.getACurrent();
  sprintf (str, "IMeter: Absolute Current:  %d", acurr);
  Serial.println(str);
/**  Displays Relative Current value
 */  
  rcurr = im.getRCurrent();
  sprintf (str, "IMeter: Relative Current:  %d",  rcurr);
  Serial.println(str);
/**  Displays Reference Current value
 */  
  refI = im.getReference();
  sprintf (str, "IMeter: Reference Current:  %d",  refI);
  Serial.println(str);
  
  Serial.println( "-------------" );
  delay (1500);
  
  
}

