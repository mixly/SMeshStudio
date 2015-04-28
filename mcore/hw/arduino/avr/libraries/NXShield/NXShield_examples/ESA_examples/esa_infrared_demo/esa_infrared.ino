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
#include <EV3InfraRed.h>


NXShield    nxshield;
EV3InfraRed ev3ir (0x32);

int inByte = 0;

void setup()
{
    char str[80];

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
    Serial.println ("Press GO button to continue");
    Serial.println ("then Press LEFT to change mode InfraRed_Beacon");
    Serial.println ("     Press RIGHT to change mode MODE_InfraRed_Remote");
    Serial.println ("     Press GO to change mode to InfraRed Proximity.");

    nxshield.waitForButtonPress(BTN_GO);
    ev3ir.init( &nxshield, SH_BAS1 );
    ev3ir.setMode(MODE_InfraRed_Proximity);

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
    char            str[256];
    byte buf[64];
    int value, v0, v1, v2, v3, h0, h1, h2, h3, p0, p1, p2, p3;
    int r0, r1, r2, r3;
    
    boolean go_btn = nxshield.getButtonState(BTN_GO);
    if ( go_btn == true ) {
        Serial.println ( "changing mode to InfraRed Proximity" );
        ev3ir.setMode(MODE_InfraRed_Proximity);
    }
    boolean left_btn = nxshield.getButtonState(BTN_LEFT);
    if ( left_btn == true ) {
        Serial.println ( "changing mode to InfraRed_Beacon" );
        ev3ir.setMode(MODE_InfraRed_Beacon);
    }

    boolean right_btn = nxshield.getButtonState(BTN_RIGHT);
    if ( right_btn == true ) {
        Serial.println ( "changing mode to MODE_InfraRed_Remote" );
        ev3ir.setMode(MODE_InfraRed_Remote);
    }
    
    char mode;
    mode = ev3ir.getMode();
    sprintf (str, "current Mode: %d", mode);
    Serial.println ( str );
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

