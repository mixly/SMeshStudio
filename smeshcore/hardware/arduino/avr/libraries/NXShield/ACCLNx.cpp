
// ACCLNx.cpp
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


#include "ACCLNx.h"

ACCLNx::ACCLNx(uint8_t i2c_address)
 : NXShieldI2C(i2c_address)
{

}

uint8_t ACCLNx::issueCommand(char command)
{
	return writeByte(ACCLNx_Command, (uint8_t)command);
}

bool ACCLNx::aquireXpointCalibration()
{
	return issueCommand('X');
}

bool ACCLNx::aquireXpointCalibrationEnd()
{
	return issueCommand('x');
}

bool ACCLNx::aquireYpointCalibration()
{
	return issueCommand('Y');
}

bool ACCLNx::aquireYpointCalibrationEnd()
{
	return issueCommand('y');
}

bool ACCLNx::aquireZpointCalibration()
{
	return issueCommand('Z');
}

bool ACCLNx::aquireZpointCalibrationEnd()
{
	return issueCommand('z');
}

bool ACCLNx::resetFactory()
{
	return issueCommand('R');
}

short ACCLNx::getXTilt()
{
	return readByte(ACCLNx_X_TILT);
}

short ACCLNx::getYTilt()
{
	return readByte(ACCLNx_Y_TILT);
}

short ACCLNx::getZTilt()
{
	return readByte(ACCLNx_Z_TILT);
}

int ACCLNx::getXAccl()
{
	return readInteger(ACCLNx_X_ACCL);
}

int ACCLNx::getYAccl()
{
	return readInteger(ACCLNx_Y_ACCL);
}

int ACCLNx::getZAccl()
{
	return readInteger(ACCLNx_Y_ACCL);
}

//Sensitivity is a value ranging from 1-4 

short ACCLNx::getSens()
{
	return readByte(READ_SENS);
}
