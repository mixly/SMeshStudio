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
 * M. Giles   01/28/14  Added I2C address, detect, and fully tested
 */


#include	"EV3Sonar.h"

EV3Sonar::EV3Sonar(uint8_t i2c_address):EV3SensorAdapter(i2c_address)
{
}
uint8_t	EV3Sonar::setMode(MODE_Sonar m)
{
    if ( m < MODE_Sonar_CM ||
        m >  MODE_Sonar_Presence ) {
       return -1;
    }    
    return EV3SensorAdapter::setMode((char) m);
}

uint8_t  EV3Sonar::detect()
{
    byte buf[17];
    readRegisters(0x54, 10, buf);
    if (buf[0] > 1){
	return 0;}
    else{
	return (buf[0]);}
}
