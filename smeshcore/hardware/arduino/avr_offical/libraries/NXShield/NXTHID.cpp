
// NXTHID.cpp
//
// This is a class for controlling the NXT Human Interface Device, made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=119.

// Initial version: 2010-06-17 by Andrew Sylvester
// Large ports of the code is ported from the NXC library for the device,
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


#include "NXTHID.h"


NXTHID::NXTHID(uint8_t i2c_address)
: NXShieldI2C(i2c_address)
{
}

uint8_t NXTHID::issueCommand(char command)
{
	return writeByte(HID_Command, (uint8_t)command);
}

bool NXTHID::transmitData()
{
	return issueCommand('T');
}

bool NXTHID::asciiMode()
{
	return issueCommand('A');
}

bool NXTHID::directMode()
{
	return issueCommand('D');
}

bool NXTHID::setModifier(uint8_t mod)
{
	uint8_t reg = HID_Modifier; 
	return writeByte(reg, (uint8_t)mod);
}

bool NXTHID::sendKeyboardData(uint8_t data)
{
	bool b;
	uint8_t reg = HID_Keyboard; 
	writeByte(reg, (uint8_t)data);
	b = transmitData();
	delay(30);
	return (b);
}

bool NXTHID::sendTextString(char *str)
{

	int i, s;
	char b;
  asciiMode();
  s = strlen(str);

  for ( i=0; i < s; i++) {
    if ( str[i] == 0x5c ) {  // that's a backslash so
      b = str[i+1];  // special treatment for next character.
      switch (b) {
        case 'r':
          sendKeyboardData(0x0a);
          break;
        case 'n':
          sendKeyboardData(0x0d);
          break;
        case 't':
          sendKeyboardData(0x09);
          break;
        default:
          break;
      }
      i++;
    }
    else {
      sendKeyboardData(str[i]);
    }
  }
}
