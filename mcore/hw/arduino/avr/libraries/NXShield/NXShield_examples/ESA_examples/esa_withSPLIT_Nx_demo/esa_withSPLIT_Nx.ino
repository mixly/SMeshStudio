/************************************************************************/
/* 
  Copyright (c) 2013 by mindsensors.com                                
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
#include <EV3Sonar.h>
#include <EV3Color.h>
#include <EV3InfraRed.h>


NXShield    nxshield;
EV3Sonar    ev3sonar (0x32);
EV3Color    ev3color (0x34);
EV3InfraRed ev3ir (0x36);

int inByte = 0;
bool sonarmode = false;
bool colormode = false;
bool irmode = false;

bool askmode()              //will ask the user to choose the mode of each sensor
{
    char str[80];  
  
    Serial.println ("     Set mode for sonar sensor");
    Serial.println ("     Enter '0' to set sonar mode to centimeters");
    Serial.println ("     Enter '1' to set sonar mode to inches");
    Serial.println ("     Enter '2' to set sonar mode to presence.");
    while (sonarmode == false ) {
      delay(100);    
      if (Serial.available() > 0) {           // get user input from serial window
      inByte = Serial.read();
      sprintf (str, "input: %c", inByte); 
      Serial.println(str);        
      switch (inByte) {
	case '0':
             sonarmode = true;
             Serial.println ( "Mode set to centimeters" );
             ev3sonar.setMode(MODE_Sonar_CM);
	     break;
        case '1':
             sonarmode = true;
	     Serial.println ( "Mode set to inches" );
             ev3sonar.setMode(MODE_Sonar_Inches);
	     break;
	case '2':
             sonarmode = true;
             Serial.println ( "Mode set to presence" );
             ev3sonar.setMode(MODE_Sonar_Presence);
	     break;
        default:
             sonarmode = false;
             sprintf (str, "invalid key");
             Serial.println(str);
             break;
         }
      }
    }
    Serial.println ("     Set mode for color sensor");
    Serial.println ("     Enter '0' to set sonar mode to reflected light");
    Serial.println ("     Enter '1' to set sonar mode to ambient light");
    Serial.println ("     Enter '2' to set sonar mode to color.");
    while (colormode == false ) {
      delay(100);    
      if (Serial.available() > 0) {           // get user input from serial window
      inByte = Serial.read();
      sprintf (str, "input: %c", inByte); 
      Serial.println(str);        
      switch (inByte) {
	case '0':
             colormode = true;
             Serial.println ( "Mode set to refelcted light" );
             ev3color.setMode(MODE_Color_ReflectedLight);
	     break;
        case '1':
             colormode = true;
	     Serial.println ( "Mode set to ambient light" );
             ev3color.setMode(MODE_Color_AmbientLight);
	     break;
	case '2':
             colormode = true;
             Serial.println ( "Mode set to color" );
             ev3color.setMode(MODE_Color_MeasureColor);
	     break;
        default:
             colormode = false;
             sprintf (str, "invalid key");
             Serial.println(str);
             break;
         }
      }
    }
    Serial.println ("     Set mode for infrared sensor");
    Serial.println ("     Enter '0' to set sonar mode to proximity");
    Serial.println ("     Enter '1' to set sonar mode to beacon");
    Serial.println ("     Enter '2' to set sonar mode to remote.");
    while (irmode == false ) {
      delay(100);    
      if (Serial.available() > 0) {           // get user input from serial window
      inByte = Serial.read();
      sprintf (str, "input: %c", inByte); 
      Serial.println(str);        
      switch (inByte) {
	case '0':
             irmode = true;
             Serial.println ( "Mode set to proximity" );
             ev3ir.setMode(MODE_InfraRed_Proximity);
	     break;
        case '1':
             irmode = true;
	     Serial.println ( "Mode set to beacon" );
             ev3ir.setMode(MODE_InfraRed_Beacon);
	     break;
	case '2':
             irmode = true;
             Serial.println ( "Mode set to remote" );
             ev3ir.setMode(MODE_InfraRed_Remote);
	     break;
        default:
             irmode = false;
             sprintf (str, "invalid key");
             Serial.println(str);
             break;
         }
      }
    }
}
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
    nxshield.init(SH_HardwareI2C);
    //
    // Wait until user presses GO button to continue the program
    //
    Serial.println ("Press GO button to start program");
    nxshield.waitForButtonPress(BTN_GO);
    
    ev3sonar.init( &nxshield, SH_BAS1 );
    ev3color.init( &nxshield, SH_BAS1 );
    ev3ir.init( &nxshield, SH_BAS1 );
    askmode();

}

void printData(char *token, byte *buf, int len, char *out)
{
    char temp[10];
    int i;
    strcpy(out, token);
    for (i=0; i < len; i++) {
        if ( buf[i] >= ' ' && buf[i] <= '~' ) {
            sprintf (temp, "'%c' ", buf[i]);
        } else {
            sprintf (temp, "x%02x ", buf[i]);
        }
        strcat(out, temp);
    }

}

void loop()
{
    char str[256];
    byte buf[64];
    int value, v0, v1, v2, v3, h0, h1, h2, h3, p0, p1, p2, p3;
    int r0, r1, r2, r3;

    char mode;                              //displays the output of the sonar sensor
    mode = ev3sonar.getMode();
    switch ( mode ) {
        case MODE_Sonar_CM:
            buf[0] = '\0';
            value = ev3sonar.readValue();
            sprintf (str, "value: %d", value/10);
            Serial.println ( str );
        break;
        case MODE_Sonar_Inches:
            buf[0] = '\0';
            value = ev3sonar.readValue();
            sprintf (str, "value: %d", value/10);
            Serial.println ( str );            
        break;
        case MODE_Sonar_Presence:
            buf[0] = '\0';
            value = ev3sonar.detect();
            sprintf (str, "value: %d", value);
            Serial.println ( str );   
        break;
        default:
        Serial.println ("unknown mode: ");             
    }
        
        buf[0] = '\0';                              //displays the output of the color sensor
        value = ev3color.readValue();
        sprintf (str, "value: %d", value);
        Serial.println ( str );
        
        mode = ev3ir.getMode();                     //displays the output of the infrared sensor
        switch ( mode ) {
          case MODE_InfraRed_Proximity:
            value = ev3ir.readValue();
            sprintf(str, "value: %d", value);
            Serial.println ( str );
          break;
          case MODE_InfraRed_Beacon:
            h0 = ev3ir.readBeaconHeading(0);
            p0 = ev3ir.readBeaconProx(0);
            r0 = ev3ir.BeaconInRange(0);
            h1 = ev3ir.readBeaconHeading(1);
            p1 = ev3ir.readBeaconProx(1);
            r1 = ev3ir.BeaconInRange(1);
            h2 = ev3ir.readBeaconHeading(2);
            p2 = ev3ir.readBeaconProx(2);
            r2 = ev3ir.BeaconInRange(2);
            h3 = ev3ir.readBeaconHeading(3);
            p3 = ev3ir.readBeaconProx(3);
            r3 = ev3ir.BeaconInRange(3);            
            sprintf(str, "values: h0: %d, p0: %d, h1: %d, p1: %d, h2: %d, p2: %d, h3: %d, p3: %d", h0,p0,h1,p1,h2,p2,h3,p3);
            Serial.println ( str );
            sprintf(str, "Is beacon within range? %d, %d, %d, %d", r0,r1,r2,r3);
            Serial.println ( str );
          break;
          case MODE_InfraRed_Remote:
            v0 = ev3ir.readButtonValue(0);
            v1 = ev3ir.readButtonValue(1);
            v2 = ev3ir.readButtonValue(2);
            v3 = ev3ir.readButtonValue(3);
            sprintf(str, "values: v0: %d, v1: %d, v2: %d, v3: %d", v0,v1,v2,v3);
            Serial.println ( str );
          break;
          default:
          Serial.println ("unknown mode: ");
      
    }


    delay(300);

}

