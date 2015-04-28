
// NXTCam.cpp
//
// This is a class for controlling the NXT Cam, made by Mindsensors.
// See http://www.mindsensors.com/index.php?module=pagemaster&PAGE_user_op=view_page&PAGE_id=78 .

// Initial version: 2010-06-18 by Andrew Sylvester
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


#include "NXTCam.h"


NXTCam::NXTCam(uint8_t i2c_address)
: NXShieldI2C(i2c_address)
{
}

uint8_t NXTCam::issueCommand(char command)
{
  return writeByte(Cam_Command, (uint8_t)command);
}

bool NXTCam::sortSize()
{
  return issueCommand('A');
}

bool NXTCam::selectObjectMode()
{
  return issueCommand('B');
}

bool NXTCam::writeImageRegisters()
{
  return issueCommand('C');
}

bool NXTCam::disableTracking()
{
  return issueCommand('D');
}

bool NXTCam::enableTracking()
{
  return issueCommand('E');
}

bool NXTCam::getColorMap()
{
  return issueCommand('G');
}

bool NXTCam::illuminationOn()
{
  return issueCommand('I');
}

bool NXTCam::readImageRegisters()
{
  return issueCommand('H');
}

bool NXTCam::selectLineMode()
{
  return issueCommand('L');
}

bool NXTCam::pingCam()
{
  return issueCommand('P');
}

bool NXTCam::resetCam()
{
  return issueCommand('R');
}

bool NXTCam::sendColorMap()
{
  return issueCommand('S');
}

bool NXTCam::illuminationOff()
{
  return issueCommand('T');
}

bool NXTCam::sortColor()
{
  return issueCommand('U');
}

bool NXTCam::camFirmware()
{
  return issueCommand('V');
}

bool NXTCam::sortNone()
{
  return issueCommand('X');
}  

int NXTCam::getNumberObjects()
{
  return readByte(Cam_Number_Objects);
}

void NXTCam::getBlobs(int *nblobs, uint8_t *color, uint8_t *left, uint8_t *top, uint8_t *right, uint8_t *bottom)
{
    *nblobs = readByte(Cam_Number_Objects);
  for (int i = 0; i < *nblobs; i++) {
    uint8_t* buf ;
    buf = (uint8_t *) readString (Start_Reg +(i*5), 5);
    color[i]  = buf[0] & 0x00FF;
    left[i]   = buf[1] & 0x00FF;
    top[i]    = buf[2] & 0x00FF;
    right[i]  = buf[3] & 0x00FF;
    bottom[i] = buf[4] & 0x00FF;
  }
}
