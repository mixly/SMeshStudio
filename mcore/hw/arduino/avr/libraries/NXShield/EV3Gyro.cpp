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
 * M. Giles   01/28/14  Added I2C address and fully tested
 */


#include	"EV3Gyro.h"

EV3Gyro::EV3Gyro(uint8_t i2c_address):EV3SensorAdapter(i2c_address)
{
}
uint8_t	EV3Gyro::setMode(MODE_Gyro m)
{
    if ( m < MODE_Gyro_Angle ||
        m >  MODE_Gyro_Rate ) {
        return -1;
    }
    uint8_t mode = EV3SensorAdapter::setMode((char) m);
    delay(500);
    return mode;
}
