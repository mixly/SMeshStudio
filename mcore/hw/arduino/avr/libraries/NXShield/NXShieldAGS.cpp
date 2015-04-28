
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

#include "NXShieldAGS.h"

NXShieldAGS::NXShieldAGS()
{
  mp_shield = NULL;
}

NXShieldAGS::NXShieldAGS(NXShield * shield, SH_BankPort bp)
{
  mp_shield = shield;
  m_bp = bp;
}

bool NXShieldAGS::setType(uint8_t type)
{
  if ( mp_shield == NULL) return false;
  switch (m_bp) {
    case SH_BAS1:
      return mp_shield->bank_a.sensorSetType(SH_S1, type);
      
    case SH_BAS2:
      return mp_shield->bank_a.sensorSetType(SH_S2, type);
     
    case SH_BBS1:
      return mp_shield->bank_b.sensorSetType(SH_S1, type);
    
    case SH_BBS2:
      return mp_shield->bank_b.sensorSetType(SH_S2, type);

  }
}

int NXShieldAGS::readRaw()
{
  if ( mp_shield == NULL) return -1;

  switch (m_bp) {
    case SH_BAS1:
      return mp_shield->bank_a.sensorReadRaw(SH_S1);

    case SH_BAS2:
      return mp_shield->bank_a.sensorReadRaw(SH_S2);

    case SH_BBS1:
      return mp_shield->bank_b.sensorReadRaw(SH_S1);

    case SH_BBS2:
      return mp_shield->bank_b.sensorReadRaw(SH_S2);

  }
}

bool NXShieldAGS::init(NXShield * shield, SH_BankPort bp)
{
  mp_shield = shield;
  m_bp = bp;
  return true;
}
