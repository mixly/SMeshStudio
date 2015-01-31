//EV3SensorAdapter.h
//
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


#ifndef	EV3_INFRARED_H
#define	EV3_INFRARED_H

#define	ESA_Command	0x41

#include	"NXShieldI2C.h"
#include	"EV3SensorAdapter.h"

/**
 * \enum MODE_InfraRed, Modes supported by EV3 InfraRed Sensor
 */
typedef enum {
  MODE_InfraRed_Proximity     = 0x00,   /*!< Choose for measuring Proximity with EV3 Infrared sensor */
  MODE_InfraRed_Beacon     = 0x01,   /*!< Choose for measuring in Mode Beacon (returns 8 bytes values - 2 per channel) */
  MODE_InfraRed_Remote     = 0x02,   /*!< Choose for Remote mode  */
} MODE_InfraRed;
	/** @brief This class implements EV3 InfraRed Sensor using EV3SensorAdapter.
      */
class EV3InfraRed : public EV3SensorAdapter
{
public:
    EV3InfraRed(uint8_t i2c_address = 0x32);
    /** Class constructor for EV3InfraRed; custom i2c address is an optional parameter */
    uint8_t setMode(MODE_InfraRed);
    /** Inrared sensor's values will be channel specific for Beacon and Remote modes. 
    Channel can be between: 0-3 */
    int8_t  readBeaconHeading(char channel);
    /** Infrared sensor will show a heading value dependent on beacon placement with respect to the sensor.
    Only used with mode 0x01 */ 
    uint8_t  readBeaconProx(char channel);
    /** Infrared sensor will show a proximity value dependent on how far the beacon is from the sensor.
    Only used with mode 0x01 */
    uint8_t  readButtonValue(char channel);
    /** Infrared sensor will read a button value of the remote beacon.
    Only used with mode 0x02 */
    uint8_t  BeaconInRange(char channel);
    /** Infrared sensor will detect if the beacon is within range.
    Only used with mode 0x01 */ 

};

#endif
