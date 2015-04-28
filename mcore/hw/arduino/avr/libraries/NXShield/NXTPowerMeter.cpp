
// NXTPowerMeter.cpp
//
// This is a class for reading from NXT Power Meter, made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=125

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


#include "NXTPowerMeter.h"


NXTPowerMeter::NXTPowerMeter(uint8_t i2c_address)
: NXShieldI2C(i2c_address)
{
}

uint8_t NXTPowerMeter::issueCommand(char command)
{
	return writeByte(NXTPowerMeter_Command, (uint8_t)command);
}

bool NXTPowerMeter::resetCounters()
{
	return issueCommand('R');
}

int NXTPowerMeter::readPresentCurrent()
{
  return readInteger(NXTPowerMeter_Present_Current);
}

int NXTPowerMeter::readPresentVoltage()
{
  return readInteger(NXTPowerMeter_Present_Voltage);
}

int NXTPowerMeter::readCapacityUsed()
{
  return readInteger(NXTPowerMeter_Capacity_Used);
}

long NXTPowerMeter::readElapsedTime()
{
	return readLong(NXTPowerMeter_Elapsed_Time);
}
