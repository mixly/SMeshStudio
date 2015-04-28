
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

#include "NXShieldI2C.h"
#include "NXShield.h"

NXShieldI2C::NXShieldI2C(uint8_t i2c_address)
  : BaseI2CDevice(i2c_address), SoftI2cMaster(i2c_address)
{
}

uint8_t  NXShieldI2C::readByte  (uint8_t location)
{
  if (!m_protocol) return BaseI2CDevice::readByte( location );
  else             return SoftI2cMaster::readByte( location );
}


uint16_t NXShieldI2C::readInteger  (uint8_t location)
{
  if (!m_protocol) return BaseI2CDevice::readInteger( location );
  else             return SoftI2cMaster::readInteger( location );
}


uint32_t NXShieldI2C::readLong  (uint8_t location)
{
  if (!m_protocol) return BaseI2CDevice::readLong( location );
  else             return SoftI2cMaster::readLong( location );
}


uint8_t*  NXShieldI2C::readRegisters  (uint8_t  startRegister, uint8_t  bytes, uint8_t* buf)
{
  if (!m_protocol) return BaseI2CDevice::readRegisters(startRegister, bytes, buf);
	else             return SoftI2cMaster::readRegisters(startRegister, bytes, buf);
}


char*    NXShieldI2C::readString  (uint8_t  location, uint8_t  bytes_to_read,
            uint8_t* buffer, uint8_t  buffer_length)
{
  if (!m_protocol) return BaseI2CDevice::readString(location, bytes_to_read, buffer, buffer_length);
  else             return SoftI2cMaster::readString(location, bytes_to_read, buffer, buffer_length);
}


bool NXShieldI2C::writeRegisters  (uint8_t start_register, uint8_t bytes_to_write, uint8_t* buffer)
{
  if (!m_protocol) return BaseI2CDevice::writeRegisters(start_register, bytes_to_write, buffer);
  else             return SoftI2cMaster::writeRegisters(start_register, bytes_to_write, buffer);
}

bool NXShieldI2C::writeByte  (uint8_t location, uint8_t data)
{
  uint8_t dd[3];
  if (!m_protocol) return BaseI2CDevice::writeByte(location, data);
  else             return SoftI2cMaster::writeByte(location, data);
}

bool NXShieldI2C::writeInteger(uint8_t location, uint16_t data)
{
  if (!m_protocol) return BaseI2CDevice::writeInteger(location, data);
  else             return SoftI2cMaster::writeInteger(location, data);
}

bool NXShieldI2C::writeLong  (uint8_t location, uint32_t data)
{
  if (!m_protocol) return BaseI2CDevice::writeLong(location, data);
  else             return SoftI2cMaster::writeLong(location, data);
}

uint8_t NXShieldI2C::getErrorCode  ( )
{
  if (!m_protocol) return BaseI2CDevice::getWriteErrorCode();
  else             return SoftI2cMaster::getWriteErrorCode();
}

bool NXShieldI2C::checkAddress  ( )
{
  if (!m_protocol) return BaseI2CDevice::checkAddress();
  else             return SoftI2cMaster::checkAddress();
}

bool NXShieldI2C::setAddress  (uint8_t address)
{
	// regardless of protocol, set the address
  BaseI2CDevice::setAddress(address);
  SoftI2cMaster::setAddress(address);
	return true;
}





// READ INFORMATION OFF OF THE DEVICE
// returns a string with the current firmware version of the device
char* NXShieldI2C::getFirmwareVersion()
{
  return readString(0, 8);
}

// returns a string indicating the vendor of the device
char* NXShieldI2C::getVendorID()
{
  return readString(0x08, 8);
}

// returns a string indicating the device's ID
char* NXShieldI2C::getDeviceID()
{
  return readString(0x10, 8);
}
// returns a string indicating the features on this device
// some devices may return null.
char* NXShieldI2C::getFeatureSet()
{
  return readString(0x18, 8);
}

void NXShieldI2C::init(void * shield, SH_BankPort bp)
{
  mp_shield = shield;
	// on BAS1 hardware as well as software protocols are possible.
	// so store the main shield's protocol value with us.
	// and initialize with appropriate function
	//
	// For BAS2, BBS1, BBS2 only software i2c is possible.

  switch (bp) {
    case SH_BAS1:
      m_protocol = ((NXShield *)shield)->m_protocol;
      switch (m_protocol) {
        case SH_HardwareI2C:
          BaseI2CDevice::initProtocol ( );
          break;
        case SH_SoftwareI2C:
          SoftI2cMaster::initProtocol ( ); // no arguments, ie use default h/w i2c pins: (A5,A4)
          break;
      }
      break;
    case SH_BAS2:
        m_protocol = SH_SoftwareI2C;
        SoftI2cMaster::initProtocol (SCL_BAS2, SDA_BAS2);
      break;
    case SH_BBS1:
        m_protocol = SH_SoftwareI2C;
        SoftI2cMaster::initProtocol (SCL_BBS1, SDA_BBS1);
      break;
    case SH_BBS2:
        m_protocol = SH_SoftwareI2C;
        SoftI2cMaster::initProtocol (SCL_BBS2, SDA_BBS2);
      break;
  }

}

