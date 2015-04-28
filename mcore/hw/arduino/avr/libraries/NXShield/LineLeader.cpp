
// LineLeader.cpp
//
// This is a class for reading from LineLeader Sensor made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=111

// Initial version: 2010-06-15 by Andrew Sylvester
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


#include "LineLeader.h"


LineLeader::LineLeader(uint8_t i2c_address)
: NXShieldI2C(i2c_address)
{
}

uint8_t LineLeader::issueCommand(char command)
{
	return writeByte(LineLeader_Command, (uint8_t)command);
}

bool LineLeader::calibrateWhite()
{
	return issueCommand('W');
}

bool LineLeader::calibrateBlack()
{
	return issueCommand('B');
}

bool LineLeader::sleep()
{
	return issueCommand('D');
}

bool LineLeader::wakeUp()
{
	return issueCommand('P');
}

bool LineLeader::invertLineColorToWhite()
{
	return issueCommand('I');
}

bool LineLeader::resetColorInversion()
{
	return issueCommand('R');
}

bool LineLeader::takeSnapshot()
{
	return issueCommand('S');
}

bool LineLeader::configureUS()
{
	return issueCommand('A');
}

bool LineLeader::configureEurope()
{
	return issueCommand('E');
}

bool LineLeader::configureUniversal()
{
	return issueCommand('U');
}

uint8_t LineLeader::getSetPoint()
{
	return readByte(LineLeader_SetPoint);
}

bool LineLeader::setSetPoint(uint8_t spoint)
{
	return writeByte(LineLeader_SetPoint, (uint8_t)spoint);
}

uint8_t LineLeader::getKp(uint8_t kp)
{
	return readByte(LineLeader_Kp);
}

bool LineLeader::setKp(uint8_t kp)
{
	return writeByte(LineLeader_Kp, (uint8_t)kp);
}

uint8_t LineLeader::getKi(uint8_t ki)
{
	return readByte(LineLeader_Ki);
}

bool LineLeader::setKi(uint8_t ki)
{
	return writeByte(LineLeader_Ki, (uint8_t)ki);
}

uint8_t LineLeader::getKd(uint8_t kd)
{
	return readByte(LineLeader_Kd);
}

bool LineLeader::setKd(uint8_t kd)
{
	return writeByte(LineLeader_Kd, (uint8_t)kd);
}

uint8_t LineLeader::getKpFactor(uint8_t kpfact)
{
	return readByte(LineLeader_Kp);
}

bool LineLeader::setKpFactor(uint8_t kpfact)
{
	return writeByte(LineLeader_Kp_Factor, (uint8_t)kpfact);
}

uint8_t LineLeader::getKiFactor(uint8_t kifact)
{
	return readByte(LineLeader_Ki);
}

bool LineLeader::setKiFactor(uint8_t kifact)
{
	return writeByte(LineLeader_Ki_Factor, (uint8_t)kifact);
}

uint8_t LineLeader::getKdFactor(uint8_t kdfact)
{
	return readByte(LineLeader_Kd);
}

bool LineLeader::setKdFactor(uint8_t kdfact)
{
	return writeByte(LineLeader_Kd_Factor, (uint8_t)kdfact);
}

int LineLeader::getSteering()
{
	return readByte(LineLeader_Steering);
}

unsigned char LineLeader::getAverage()
{
	return readByte(LineLeader_Average);
}

unsigned char LineLeader::getResult()
{
	return readByte(LineLeader_Result);
}

uint8_t* LineLeader::getRawCalibrated()
{
	return (uint8_t *) readString (LineLeader_Raw_Calibrated, 8);
}

uint8_t* LineLeader::getRawUncalibrated()
{
	return (uint8_t *) readString (LineLeader_Raw_Uncalibrated, 16);
}

uint8_t* LineLeader::getWhiteLimit()
{
	return (uint8_t *)readString (LineLeader_White_Limit, 8);
}

uint8_t* LineLeader::getBlackLimit()
{
	return (uint8_t *)readString (LineLeader_Black_Limit, 8);
}

uint8_t* LineLeader::getWhiteCalibration()
{
	return (uint8_t *)readString (LineLeader_White_Calibration, 8);
}

uint8_t* LineLeader::getBlackCalibration()
{
	return (uint8_t *)readString (LineLeader_Black_Calibration, 8);
}
