//EV3SensorAdapter.cpp
//
// This is a class controlling the EV3SensorAdapter, made by Mindsensors.

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
 * History
 * ------------------------------------------------
 * Author     Date      Comments
 * Deepak     11/21/13  Initial Authoring.
 * M. Giles   01/28/14  Added I2C address, prox, button, beacon, and fully tested
 */


#include	"EV3InfraRed.h"

EV3InfraRed::EV3InfraRed(uint8_t i2c_address):EV3SensorAdapter(i2c_address)
{
}
uint8_t	EV3InfraRed::setMode(MODE_InfraRed m)
{
    if ( m < MODE_InfraRed_Proximity ||
        m >  MODE_InfraRed_Remote ) {
        return -1;
    }
    uint8_t mode = EV3SensorAdapter::setMode((char) m);
    delay(500);
    return mode;
}

int8_t	EV3InfraRed::readBeaconHeading(char c)
{
    byte buf[17];
    if ( c < 0 || c > 3 ) return -1;

    readRegisters(0x54, 10, buf);
    return (buf[c*2] + (buf[(c*2)]*0x100));
}

uint8_t EV3InfraRed::readBeaconProx(char c)
{
    byte buf[17];
    int prox;
    if ( c < 0 || c > 3 ) return -1;

    readRegisters(0x54, 10, buf);
    prox = (buf[c*2] + buf[(c*2)+1]);
    if (prox == 128) return 0;
    else 
	return prox;
}

uint8_t	EV3InfraRed::readButtonValue(char c)
{
    byte buf[17];
    if ( c < 0 || c > 3 ) return -1;

    readRegisters(0x54, 10, buf);
    return (buf[c]);
}

uint8_t EV3InfraRed::BeaconInRange(char c)
{
    byte buf[17];
    int valid;
    if ( c < 0 || c > 3 ) return -1;

    valid = EV3InfraRed::readBeaconProx(c);
    if (valid < 101 && valid > 0 ) return 1;
    else 
	return 0;
}
