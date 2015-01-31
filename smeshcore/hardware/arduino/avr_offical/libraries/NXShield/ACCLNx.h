
// ACCLNx.h
//
// This is a class for reading from ACCL-Nx, made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=101

// Initial version: 2010-06-10 by Andrew Sylvester
// Large parts of the code is ported from the NXC library for the device,
// written by Deepak Patil.

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


#ifndef ACCLNx_H
#define ACCLNx_H

#define ACCLNx_Command      0x41
#define READ_SENS      0x19

#define ACCLNx_X_TILT    0x42
#define ACCLNx_Y_TILT    0x43
#define ACCLNx_Z_TILT    0x44
#define ACCLNx_X_ACCL    0X45
#define ACCLNx_Y_ACCL    0X47
#define ACCLNx_Z_ACCL    0X49

#include "NXShieldI2C.h"

/**
  @brief This class interfaces with ACCL-Nx attached to NXShield 
	*/
class ACCLNx : public NXShieldI2C
{
public:
	/** constructor for the ACCLNx class; may supply an optional custom i2c address */
  ACCLNx(uint8_t i2c_address = 0x02);

	/** write a command byte at the command register of the device */  
  uint8_t    issueCommand(char command);
	
	/** begin acquisition of X axis calibration information from sensor's environment */
  bool    aquireXpointCalibration();

	/** end acquisition of X axis calibration information */
	bool    aquireXpointCalibrationEnd();

	/** begin acquisition of Y axis calibration information from sensor's environment */
  bool    aquireYpointCalibration();

	/** end acquisition of Y axis calibration information */
  bool    aquireYpointCalibrationEnd();

	/** begin acquisition of Z axis calibration information from sensor's environment */
  bool    aquireZpointCalibration();

	/** end acquisition of Z axis calibration information */
  bool    aquireZpointCalibrationEnd();
  
	/** reset device to factory default calibration settings */
	bool    resetFactory();
  
	/** get the tilt in the x-axis direction */
	short    getXTilt();
  
	/** get the tilt in the y-axis direction */
	short    getYTilt();
	
	/** get the tilt in the z-axis direction */
  short    getZTilt();
	
	/** get the acceleration in the x-axis direction */
	int      getXAccl();
  
	/** get the acceleration in the y-axis direction */
	int      getYAccl();
  
	/** get the acceleration in the z-axis direction */
	int      getZAccl();
	
	/** Get currently configured sensitivity */
  short    getSens();

};

#endif
