
// NXTHID.h
//
// This is a class for controlling the NXT Human Interface Device, made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=119.

// Initial version: 2010-06-17 by Andrew Sylvester
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


#ifndef NXTHID_H
#define NXTHID_H

#define HID_Command		 0x41
#define HID_Modifier     0x42
#define HID_Text         0x04
#define HID_Keyboard     0x43

#define HID_None       0x00
#define HID_L_Ctrl     0x01
#define HID_L_Shift    0x02
#define HID_L_Alt      0x04
#define HID_L_GUI      0x08
#define HID_R_Ctrl     0x10
#define HID_R_Shift    0x20
#define HID_R_Alt      0x40
#define HID_R_GUI      0x80

#include "NXShieldI2C.h"

/**
  @brief This class interfaces with NXTHID attached to NXShield 
	*/
class NXTHID : public NXShieldI2C
{
public:
	/** class constructor for the NXTHID; optional custom i2c address can be supplied as a parameter */
	NXTHID(uint8_t i2c_address = 0x04);

	/** issue char command byte to the NXTHID command register */
	uint8_t		issueCommand(char command);
	
	/** transmit data to host computer */
	bool		transmitData();
	
	/** set NXTHID to ascii mode; printable ascii text characters can be to the host one at a time */
	bool		asciiMode();
	
	/** set NXTHID to direct mode; non printable keys can be sent to host one at a time */
	bool		directMode();
	
	/** set keyboard modifier in direct mode (HID_None, L_Ctrl, L_Shift ,  L_Alt, L_GUI, R_Ctrl, R_Shift, R_Alt, R_GUI) */
	bool		setModifier(uint8_t mod);
	
	/** send specific key data */
	bool		sendKeyboardData(uint8_t data);
	
	/** send a string of text to the host */
	bool		sendTextString(char *str);

};

#endif
