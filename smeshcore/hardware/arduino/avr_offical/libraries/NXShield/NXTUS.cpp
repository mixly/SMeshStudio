
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

#include "NXTUS.h"

NXTUS::NXTUS(uint8_t i2c_address) : SoftI2cMaster(i2c_address)
{
	writeByte(0x41,0x02);
	mp_shield = NULL;
}

void NXTUS::init(NXShield * shield, SH_BankPort bp)
{
	mp_shield = shield;
	m_bp = bp;

	// set 9 volts on this specific port.
	if ( mp_shield == NULL) return;
	switch (m_bp) {
	  case SH_BAS1:
			SoftI2cMaster::initProtocol (); // no arguments, ie use default h/w i2c pins.
		  mp_shield->bank_a.sensorSetType(SH_S1, SH_Type_ANALOG_9VOLTS);
			break;
		  
	  case SH_BAS2:
			SoftI2cMaster::initProtocol (SCL_BAS2, SDA_BAS2);
		  mp_shield->bank_a.sensorSetType(SH_S2, SH_Type_ANALOG_9VOLTS);
			break;
		 
	  case SH_BBS1:
			SoftI2cMaster::initProtocol (SCL_BBS1, SDA_BBS1);
		  mp_shield->bank_b.sensorSetType(SH_S1, SH_Type_ANALOG_9VOLTS);
			break;
		
	  case SH_BBS2:
			SoftI2cMaster::initProtocol (SCL_BBS2, SDA_BBS2);
		  mp_shield->bank_b.sensorSetType(SH_S2, SH_Type_ANALOG_9VOLTS);
			break;
	
	}
}

/*
NXTUS::NXTUS(uint8_t i2c_address, bool useSoftI2c) : SoftI2cMaster(i2c_address)
{
	uint8_t buf[2] = {0x41, 0x02};
		init();
		writeRegistersWithLocation(2,buf);
}
*/

uint8_t NXTUS::getDist(){
	writeByte(0x41,0x02);
	delay(20);  // required for ultrasonic to work.
	return readByte(0x42);
}

char*		NXTUS::getFirmwareVersion()
{
	delay(4);  // required for ultrasonic to work.
	return SoftI2cMaster::getFirmwareVersion();
}

char*		NXTUS::getVendorID()
{
	delay(3);  // required for ultrasonic to work.
	return SoftI2cMaster::getVendorID();
}


char*		NXTUS::getDeviceID()
{
	delay(6);  // required for ultrasonic to work.
	return SoftI2cMaster::getDeviceID();
}


