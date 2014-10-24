/// @file Watchdog.cpp provides the interface to the Watchdog module
///
/// This provides basic Watchdog service for the mbed. You can configure
/// various timeout intervals that meet your system needs. Additionally,
/// it is possible to identify if the Watchdog was the cause of any 
/// system restart.
/// 
/// Adapted from Simon's Watchdog code from http://mbed.org/forum/mbed/topic/508/
///
/// @note Copyright &copy; 2011 by Smartware Computing, all rights reserved.
///     This software may be used to derive new software, as long as
///     this copyright statement remains in the source file.
/// @author David Smart
///
//#include "mbed.h"
#include "dev/watchdog.h"
#include "LPC17xx.h"
//cause mbed it release watchdog, we just blank it temporary.
void
watchdog_init(void)
{
	//	Watchdog.Configure(2);
	LPC_WDT->WDCLKSEL = 0x1;                // Set CLK src to PCLK
	uint32_t clk = SystemCoreClock / 16;    // WD has a fixed /4 prescaler, PCLK default is /4
	LPC_WDT->WDTC = 4 * (float)clk;


}
/*---------------------------------------------------------------------------*/
void
watchdog_start(void)
{
	/*
	 * We setup the watchdog to reset the device after 2.048 seconds,
	 * unless watchdog_periodic() is called.
	 */
	//	Watchdog.Configure(2);
	LPC_WDT->WDMOD = 0x3;                   // Enabled and Reset
}
/*---------------------------------------------------------------------------*/
void
watchdog_periodic(void)
{
	/* This function is called periodically to restart the watchdog timer. */
	//  Watchdog.Service();
	LPC_WDT->WDFEED = 0xAA;
	LPC_WDT->WDFEED = 0x55;
}
/*---------------------------------------------------------------------------*/
void
watchdog_stop(void)
{
	//  halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);
	LPC_WDT->WDMOD = 0x0;
}
/*---------------------------------------------------------------------------*/
void
watchdog_reboot(void)
{
	//  halReboot();
	LPC_WDT->WDMOD = 0x11;
}
