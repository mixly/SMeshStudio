#include <Wire.h>
#include <NXShield.h>
#include <NXTVoltMeter.h>

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

VoltMeter  vm  (0x26);

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
  vm.init( &nxshield, SH_BAS1 );
/**  Set the Reference Voltage to Absolute Voltage.
 */
   vm.setReferenceV();

}


#define DELAY_T 0

void loop()
{
  char  str[256];      //declares character string length
  int  avolt;          //declares "avolt" variable
  int  rvolt;          //declares "rvolt" variable
  int  refV;           //declares "refV" variable
  
/**  Displays Absolute Voltage value
 */
  avolt = vm.getAVoltage();
  sprintf (str, "VMeter: Absolute Volts:   %d", avolt);
  Serial.println(str);
/**  Displays Relative Voltage value
 */  
  rvolt = vm.getRVoltage();
  sprintf (str, "VMeter: Relative Volts:   %d", rvolt);
  Serial.println(str);
/**  Displays Reference Voltage value
 */
  refV = vm.getReference();
  sprintf (str, "VMeter: Reference Volts: %d", refV);
  Serial.println(str);
  
  Serial.println( "-------------" );
  delay (1500);
  
  
}

