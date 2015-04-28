/*
 * Copyright (c) 2012, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
 /**
 *  \brief This module contains AVR-specific code to implement
 *  the Contiki core clock functions.
 *  
 *  \author David Kopf <dak664@embarqmail.com> and others.
 *
*/
/** \addtogroup avr
 * @{
 */
 /**
 *  \defgroup avrclock AVR clock implementation
 * @{
 */
/**
 *  \file
 *  This file contains AVR-specific code to implement the Contiki core clock functions.
 *
 */
/**
 * These routines define the AVR-specific calls declared in /core/sys/clock.h
 * CLOCK_SECOND is the number of ticks per second.
 * It is defined through CONF_CLOCK_SECOND in the contiki-conf.h for each platform.
 * The usual AVR defaults are 128 or 125 ticks per second, counting a prescaled CPU clock
 * using the 8 bit timer0.
 * 
 * clock_time_t is usually declared by the platform as an unsigned 16 bit data type,
 * thus intervals up to 512 or 524 seconds can be measured with ~8 millisecond precision.
 * For longer intervals the 32 bit clock_seconds() is available.
 * 
 * Since a carry to a higer byte can occur during an interrupt, declaring them non-static
 * for direct examination can cause occasional time reversals!
 *
 * clock-avr.h contains the specific setup code for each mcu.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/clock.h"
#include "sys/etimer.h"

#ifdef __cplusplus
}
#endif

#include "mbed.h"


Ticker clock_ticker;

///* Two tick counters avoid a software divide when CLOCK_SECOND is not a power of two. */
//#if CLOCK_SECOND && (CLOCK_SECOND - 1)
//#define TWO_COUNTERS 1
//#endif

/* count is usually a 16 bit variable, although the platform can declare it otherwise */
static volatile clock_time_t count;
#if TWO_COUNTERS
/* scount is the 8 bit counter that counts ticks modulo CLOCK_SECONDS */
static volatile uint8_t scount;
#endif
/* seconds is available globally but non-atomic update during interrupt can cause time reversals */
volatile unsigned long seconds;
/* sleepseconds is the number of seconds sleeping since startup, available globally */
long sleepseconds;

/* Set RADIOSTATS to monitor radio on time (must also be set in the radio driver) */
#if RF230BB && AVR_WEBSERVER
#define RADIOSTATS 1
#endif

#if RADIOSTATS
static volatile uint8_t rcount;
volatile unsigned long radioontime;
extern uint8_t radio_receive_on;
#endif

/* Set RADIO_CONF_CALIBRATE_INTERVAL for periodic calibration of the PLL during extended radio on time.
 * The RF230 data sheet suggests every 5 minutes if the temperature is fluctuating.
 * At present the specified interval is ignored, and an 8 bit counter gives 256 second intervals.
 * Actual calibration is done by the driver on the next transmit request.
 */
#if RADIO_CONF_CALIBRATE_INTERVAL
extern volatile uint8_t rf230_calibrate;
static uint8_t calibrate_interval;
#endif

#define CLOCK_CHECK_TICKER    (1000000/CLOCK_SECOND)

void clock_ticker_check()
{

    count++;
#if TWO_COUNTERS
  if(++scount >= CLOCK_SECOND) {
    scount = 0;
#else
  if(count%CLOCK_SECOND==0) {
#endif
    seconds++;

#if RADIO_CONF_CALIBRATE_INTERVAL
   /* Force a radio PLL frequency calibration every 256 seconds */
    if (++calibrate_interval==0) {
      rf230_calibrate=1;
    }
#endif

  }

#if RADIOSTATS
   /* Sample radio on time. Less accurate than ENERGEST but a smaller footprint */
  if (radio_receive_on) {
    if (++rcount >= CLOCK_SECOND) {
      rcount=0;
      radioontime++;
    }
  }
#endif


#if 1
/*  gcc will save all registers on the stack if an external routine is called */
  if(etimer_pending()) {
    etimer_request_poll();
  }
#else
/* doing this locally saves 9 pushes and 9 pops, but these etimer.c and process.c variables have to lose the static qualifier */
  extern struct etimer *timerlist;
  extern volatile unsigned char poll_requested;

#define PROCESS_STATE_NONE        0
#define PROCESS_STATE_RUNNING     1
#define PROCESS_STATE_CALLED      2

  if (timerlist) {
    if(etimer_process.state == PROCESS_STATE_RUNNING || etimer_process.state == PROCESS_STATE_CALLED) {
      etimer_process.needspoll = 1;
      poll_requested = 1;
    }
  }
#endif
}

/*---------------------------------------------------------------------------*/
/**
 * Start the clock by enabling the timer comparison interrupts.
 */
void
clock_init(void)
{
	clock_ticker.attach_us(&clock_ticker_check,CLOCK_CHECK_TICKER);
}
/*---------------------------------------------------------------------------*/
/**
 * Return the tick counter. When 16 bit it typically wraps every 10 minutes.
 * The comparison avoids the need to disable clock interrupts for an atomic
 * read of the multi-byte variable.
 */
clock_time_t
clock_time(void)
{
  clock_time_t tmp;
  do {
    tmp = count;
  } while(tmp != count);
  return count;
}
/*---------------------------------------------------------------------------*/
/**
 * Return seconds, default is time since startup.
 * The comparison avoids the need to disable clock interrupts for an atomic
 * read of the four-byte variable.
 */
unsigned long
clock_seconds(void)
{
//  unsigned long tmp;
//  do {
//    tmp = time(NULL);
//  } while(tmp != time(NULL));
  return seconds;
}
/*---------------------------------------------------------------------------*/
/**
 * Set seconds, e.g. to a standard epoch for an absolute date/time.
 */
inline void
clock_set_seconds(unsigned long sec)
{
	seconds = sec;
}
///*---------------------------------------------------------------------------*/
///**
// * Wait for a number of clock ticks.
// */
void
clock_wait(clock_time_t t)
{
  clock_time_t endticks = clock_time() + t;
  if (sizeof(clock_time_t) == 1) {
    while ((signed char )(clock_time() - endticks) < 0) {;}
  } else if (sizeof(clock_time_t) == 2) {
    while ((signed short)(clock_time() - endticks) < 0) {;}
  } else {
    while ((signed long )(clock_time() - endticks) < 0) {;}
  }
}
/*---------------------------------------------------------------------------*/
/**
 * Delay the CPU for up to 65535*(4000000/F_CPU) microseconds.
 * Copied from _delay_loop_2 in AVR library delay_basic.h, 4 clocks per loop.
 * For accurate short delays, inline _delay_loop_2 in the caller, use a constant
 * value for the delay, and disable interrupts if necessary.
 */
//static inline void my_delay_loop_2(uint16_t __count) __attribute__((always_inline));
//void
//my_delay_loop_2(uint16_t __count)
//{
//  __asm__ volatile (
//    "1: sbiw %0,1" "\n\t"
//    "brne 1b"
//    : "=w" (__count)
//    : "0" (__count)
//  );
//}
inline void
clock_delay_usec(uint16_t howlong)
{
	wait_us(howlong);
}
//#if 1
///*---------------------------------------------------------------------------*/
///**
// * Legacy delay. The original clock_delay for the msp430 used a granularity
// * of 2.83 usec. This approximates that delay for values up to 1456 usec.
// * (The largest core call in leds.c uses 400).
// */
//void
//clock_delay(unsigned int howlong)
//{
//  if(howlong<2) return;
//  clock_delay_usec((45*howlong)>>4);
//}
//#endif
/*---------------------------------------------------------------------------*/
/**
 * Delay up to 65535 milliseconds.
 * \param dt   How many milliseconds to delay.
 *
 * Neither interrupts nor the watchdog timer is disabled over the delay.
 * Platforms are not required to implement this call.
 * \note This will break for CPUs clocked above 260 MHz.
 */
//inline void
//clock_delay_msec(uint16_t howlong)
//{
//	wait_ms(howlong);
//}
/*---------------------------------------------------------------------------*/
/**
 * Adjust the system current clock time.
 * \param dt   How many ticks to add
 *
 * Typically used to add ticks after an MCU sleep
 * clock_seconds will increment if necessary to reflect the tick addition.
  * Leap ticks or seconds can (rarely) be introduced if the ISR is not blocked.
 */
//void
//clock_adjust_ticks(clock_time_t howmany)
//{
//  uint8_t sreg = SREG;
//  __disable_irq();
////  count  += howmany;
//#if TWO_COUNTERS
//  howmany+= scount;
//#endif
////  while(howmany >= CLOCK_SECOND) {
////    howmany -= CLOCK_SECOND;
////    seconds++;
////    sleepseconds++;
//#if RADIOSTATS
//    if (radio_receive_on) radioontime += 1;
//#endif
////  }
//#if TWO_COUNTERS
//  scount = howmany;
//#endif
////  SREG=sreg;
//}

/** @} */
/** @} */
