
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

#ifndef NXTUS_H
#define NXTUS_H
#include "NXShieldAGS.h"
#include "NXShieldI2C.h"
#include "NXShield.h"

#define NXTUS_Command      0x41
#define NXTUS_Distance    0x42

/**
  @brief This class interfaces with LEGO Ultrasonic sensor attached to NXShield 
	*/
class NXTUS : public SoftI2cMaster
{
  NXShield *mp_shield;
  SH_BankPort m_bp;

public:
	/** class constructor for NXTUS with default i2c address that cannot be changed */
  NXTUS(uint8_t devAddr = 0x02);
	
	/** initialize the NXTUS on the specified NXShield and bankport */
  void init(NXShield * shield, SH_BankPort bp=SH_BBS2);
	
	/** get the distance from the NXTUS in millimeters */
  uint8_t getDist();

	/** get the firmware version of the NXTUS */
	char*		getFirmwareVersion();
	
	/** get the vendor name of the NXTUS */
	char*		getVendorID();
	
	/** get the device name of the NXTUS */
	char*		getDeviceID();
};

#endif
