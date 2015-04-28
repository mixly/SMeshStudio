
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


#ifndef RCXLIGHT_H
#define RCXLIGHT_H

#include "NXShieldAGS.h"

/*
 * NOTE: NXShield Port BAS1 is High speed I2C port, and 
 * Light sensor does not work on BAS1
 */

/**
  @brief This class interfaces with RCX Light sensor attached to NXShield.

	Note, the light sensor does not work on BAS1.
	*/
class RCXLight : public NXShieldAGS
{
public:
	/** initialize the RCXLight sensor with a pointer to the NXShield and the bank port it is connected to */
	bool init(NXShield * shield, SH_BankPort bp);

};

#endif


