/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include "Reset.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((long_call, section (".ramfunc")))
static void banzai() {
    // Disable all interrupts
    __disable_irq();

  	WDT->CONFIG.bit.PER = 0x0; //8 clock cycles period for WDT
	WDT->CTRL.reg |= WDT_CTRL_ENABLE; //enable watchdog
	while(1)
	{
		//wait for WDT
	}
	while (true);
}

static int ticks = -1;

void initiateReset(int _ticks) {
	ticks = _ticks;
	banzai();
}

void cancelReset() {
	ticks = -1;
}

void tickReset() {
	if (ticks == -1)
		return;
	ticks--;
	if (ticks == 0)
		banzai();
}

#ifdef __cplusplus
}
#endif
