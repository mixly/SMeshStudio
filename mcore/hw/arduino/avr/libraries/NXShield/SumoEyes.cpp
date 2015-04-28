
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

#include "SumoEyes.h"

bool SumoEyes::init(NXShield * shield, SH_BankPort bp)
{
  NXShieldAGS::init(shield, bp);
}
  

bool SumoEyes::setType(int8_t type)
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

bool SumoEyes::setShortRange()
{
  return setType( SH_Type_ANALOG | SH_Type_DATABIT0_HIGH );
}

bool SumoEyes::setLongRange()
{
  return setType( SH_Type_ANALOG );
}

bool SumoEyes::isNear(int reference, int delta, int comet)
{
  if ( (comet > (reference - delta))
    && (comet < (reference + delta)) ) {
       return true;
  }
  return false;
}

SE_Zone SumoEyes::detectObstacleZone()
{
  int se_value;
  
  se_value = readRaw();

  if ( isNear(344, 10, se_value) ) return SE_Front;
  if ( isNear(658, 10, se_value) ) return SE_Left;
  if ( isNear(424, 10, se_value) ) return SE_Right;

  return (SE_None);
}

char *SumoEyes::OBZoneToString(SE_Zone ob)
{
  switch (ob) {
    case SE_None:
    return "NONE";
    break;
    case SE_Front:
    return "FRONT";
    break;
    case SE_Left:
    return "LEFT";
    break;
    case SE_Right:
    return "RIGHT";
    break;
  }
}
