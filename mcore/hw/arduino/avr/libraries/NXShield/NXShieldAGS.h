
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

#ifndef NXShieldAGS_H
#define NXShieldAGS_H

#include "SHDefines.h"
#include "NXShield.h"

/**
	@brief NXShield Analog Sensor class.
  */
class NXShieldAGS
{
public:
	/** pointer to the NXShield class instantiation used */
	NXShield * mp_shield;
	
	/** bank port the analog device is connected to */
	SH_BankPort m_bp;
	
	/** null constructor for the NXShieldAGS class; need to init later */
  NXShieldAGS();
	
	/** class constructor with pointed to NXShield and the bankport as a parameter; init is not needed */
  NXShieldAGS(NXShield * shield, SH_BankPort bp);
	
	/** set the type of the device on this port of the NXShield */
  bool  setType(uint8_t type);
	
	/** read the raw analog value from the device and return as an integer */
  int   readRaw();
	
	/** initialize the analog device with a pointed to the NXShield and the bank port it is connected to */
	bool init(NXShield * shield, SH_BankPort bp);

};


#endif

