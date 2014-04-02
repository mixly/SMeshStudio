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

//cause mbed it release watchdog, we just blank it temporary.
void
watchdog_init(void)
{
//	Watchdog.Configure(2);
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
}
/*---------------------------------------------------------------------------*/
void
watchdog_periodic(void)
{
  /* This function is called periodically to restart the watchdog timer. */
//  Watchdog.Service();
}
/*---------------------------------------------------------------------------*/
void
watchdog_stop(void)
{
//  halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);
}
/*---------------------------------------------------------------------------*/
void
watchdog_reboot(void)
{
//  halReboot();
}
