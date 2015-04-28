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
 * M. Giles   01/28/14  Added I2C address, detect, and fully tested
 */


#ifndef	EV3_SONAR_H
#define	EV3_SONAR_H

#define	ESA_Command	0x41

#include	"NXShieldI2C.h"
#include	"EV3SensorAdapter.h"

/**
 * \enum MODE_Sonar, Modes supported by EV3 Sonar (Ultrasonic Sensor).
 */
typedef enum {
  MODE_Sonar_CM     = 0x00,   /*!< Choose for measurements in centimeters */
  MODE_Sonar_Inches     = 0x01,   /*!< Choose measurements in inches */
  MODE_Sonar_Presence     = 0x02,   /*!< Choose for presence detection mode */
} MODE_Sonar;
	/** @brief This class implements EV3 Ultrasonic Sensor using EV3SensorAdapter.
      Note that the distances returned are 10 times the units,
      i.e. when units are MODE_Sonar_CM, for obstacle at 5 centimeters it will return you 50 */
class EV3Sonar : public EV3SensorAdapter
{
public:
    EV3Sonar(uint8_t i2c_address = 0x32);
	 /** Class constructor for EV3Sonar; custom i2c address is an optional parameter */
    uint8_t setMode(MODE_Sonar);
    /** Sonar sensor's values will be specific for Proximity in Centimeters, Proximity in Inches, and Presence modes. */
    uint8_t detect();
    /** Sonar sensor can detect if there are any devices emiiting an ultrasonic frequency within range.
     * Only use with mode 0x02 */ 

};

#endif
