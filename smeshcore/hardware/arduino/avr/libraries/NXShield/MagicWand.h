
/*
 * NXShield interface library
 * Copyright (C) 2011 mindsensors.com
 *
 * This file is part of NXShield interface library.
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

#ifndef MagicWand_H
#define MagicWand_H
#include "NXShieldI2C.h"

#define MagicWand_WR_Register     0x00

/**
  @brief This class interfaces with Magic Wand attached to NXShield 
	*/
class MagicWand : public NXShieldI2C
{

public:
	/** class constructor for the MagicWand; the i2c address for this device cannot be changed */
  MagicWand(uint8_t devAddr = 0x70);

	/** light the want; each bit of the byteToWrite represents one LED; 0 is on and 1 is off */
  void lightWand(uint8_t byteToWrite);
};

#endif
