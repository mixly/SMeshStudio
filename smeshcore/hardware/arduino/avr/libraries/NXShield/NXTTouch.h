
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

#ifndef NXTTOUCH_H
#define NXTTOUCH_H

#include "NXShieldAGS.h"

/**
  @brief This class interfaces with LEGO Touch sensor attached to NXShield 
	*/
class NXTTouch : public NXShieldAGS
{
public:
	/** check if the touch sensor is pressed */
  bool isPressed();
};

#endif

