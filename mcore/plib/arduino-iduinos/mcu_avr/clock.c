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
#include "sys/clock.h"
#include "clock-avr.h"
#include "sys/etimer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


static volatile clock_time_t count;
volatile unsigned short milliseconds;
volatile unsigned long seconds;
/* sleepseconds is the number of seconds sleeping since startup, available globally */
long sleepseconds;


/*---------------------------------------------------------------------------*/
/**
 * Start the clock by enabling the timer comparison interrupts. 
 */
void
clock_init(void)
{
  cli ();
  OCRSetup();
  sei ();
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
  return tmp;
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
  unsigned long tmp;
  do {
    tmp = seconds;
  } while(tmp != seconds);
  return tmp;
}
/*---------------------------------------------------------------------------*/
/**
 * Set seconds, e.g. to a standard epoch for an absolute date/time.
 */
void
clock_set_seconds(unsigned long sec)
{
  seconds = sec;
}
/*---------------------------------------------------------------------------*/
/**
 * Wait for a number of clock ticks.
 */
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
static inline void my_delay_loop_2(uint16_t __count) __attribute__((always_inline));
void
my_delay_loop_2(uint16_t __count)
{
  __asm__ volatile (
    "1: sbiw %0,1" "\n\t"
    "brne 1b"
    : "=w" (__count)
    : "0" (__count)
  );
}
void
clock_delay_usec(uint16_t howlong)
{
#if 0
/* Accurate delay at any frequency, but introduces a 64 bit intermediate
  * and has a 279 clock overhead.
 */
  if(howlong<=(uint16_t)(279000000UL/F_CPU)) return;
  howlong-=(uint16_t) (279000000UL/F_CPU);
  my_delay_loop_2(((uint64_t)(howlong) * (uint64_t) F_CPU) / 4000000ULL);
  /* Remaining numbers tweaked for the breakpoint CPU frequencies */
  /* Add other frequencies as necessary */
#elif F_CPU>=16000000UL
  if(howlong<1) return;
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/3250000)));
#elif F_CPU >= 12000000UL
  if(howlong<2) return;
  howlong-=(uint16_t) (3*12000000/F_CPU);
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/3250000)));
#elif F_CPU >= 8000000UL
  if(howlong<4) return;
  howlong-=(uint16_t) (3*8000000/F_CPU);
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/2000000))/2);
#elif F_CPU >= 4000000UL
  if(howlong<5) return;
  howlong-=(uint16_t) (4*4000000/F_CPU);
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/2000000))/2);
#elif F_CPU >= 2000000UL
  if(howlong<11) return;
  howlong-=(uint16_t) (10*2000000/F_CPU);
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/1000000))/4);
#elif F_CPU >= 1000000UL
  if(howlong<=17) return;
  howlong-=(uint16_t) (17*1000000/F_CPU);
  my_delay_loop_2((howlong*(uint16_t)(F_CPU/1000000))/4);
#else
  howlong >> 5;
  if (howlong < 1) return;
  my_delay_loop_2(howlong);
#endif
}
#if 1
/*---------------------------------------------------------------------------*/
/**
 * Legacy delay. The original clock_delay for the msp430 used a granularity
 * of 2.83 usec. This approximates that delay for values up to 1456 usec.
 * (The largest core call in leds.c uses 400).
 */
void
clock_delay(unsigned int howlong)
{
  if(howlong<2) return;
  clock_delay_usec((45*howlong)>>4);
}
#endif
/*---------------------------------------------------------------------------*/
/**
 * Delay up to 65535 milliseconds.
 * \param dt   How many milliseconds to delay.
 *
 * Neither interrupts nor the watchdog timer is disabled over the delay.
 * Platforms are not required to implement this call.
 * \note This will break for CPUs clocked above 260 MHz.
 */
void
clock_delay_msec(uint16_t howlong)
{

#if F_CPU>=16000000
  while(howlong--) clock_delay_usec(1000);
#elif F_CPU>=8000000
  uint16_t i=996;
  while(howlong--) {clock_delay_usec(i);i=999;}
#elif F_CPU>=4000000
  uint16_t i=992;
  while(howlong--) {clock_delay_usec(i);i=999;}
#elif F_CPU>=2000000
  uint16_t i=989;
  while(howlong--) {clock_delay_usec(i);i=999;}
#else
  uint16_t i=983;
  while(howlong--) {clock_delay_usec(i);i=999;}
#endif
}
/*---------------------------------------------------------------------------*/
/**
 * Adjust the system current clock time.
 * \param dt   How many ticks to add
 *
 * Typically used to add ticks after an MCU sleep
 * clock_seconds will increment if necessary to reflect the tick addition.
  * Leap ticks or seconds can (rarely) be introduced if the ISR is not blocked.
 */
void
clock_adjust_ticks(clock_time_t howmany)
{
  uint8_t sreg = SREG;cli();
  count  += howmany;
#if TWO_COUNTERS
  howmany+= scount;
#endif
  while(howmany >= CLOCK_SECOND) {
    howmany -= CLOCK_SECOND;
    seconds++;
    sleepseconds++;
#if RADIOSTATS
    if (RF230_receive_on) radioontime += 1;
#endif
  }
#if TWO_COUNTERS
  scount = howmany;
#endif
  SREG=sreg;
}
void
clock_adjust_ms(unsigned long howmany)
{
	uint8_t sreg = SREG;cli();
	count  +=howmany*CLOCK_SECOND/1024;
	milliseconds=howmany  +milliseconds;
	while(milliseconds >= 1024) {
		milliseconds -= 1024;
		seconds++;
	}
	SREG=sreg;
}

/*---------------------------------------------------------------------------*/
void
clock_arch_sleepms(unsigned char howlong)
{
	/* Deep Sleep for howlong rtimer ticks. This will stop all timers except
	 * for TIMER2 which can be clocked using an external crystal.
	 * Unfortunately this is an 8 bit timer; a lower prescaler gives higher
	 * precision but smaller maximum sleep time.
	 * Here a maximum 128msec (contikimac 8Hz channel check sleep) is assumed.
	 * The rtimer and system clocks are adjusted to reflect the sleep time.
	 */
#include <avr/sleep.h>
#include <dev/watchdog.h>

#if AVR_CONF_USE32KCRYSTAL
	/* Save TIMER2 configuration for clock.c is using it */
	uint8_t savedTCNT2=TCNT2, savedTCCR2A=TCCR2A, savedTCCR2B = TCCR2B, savedOCR2A = OCR2A;
#endif
	cli();
	watchdog_stop();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	TCCR2A = _BV(WGM21);
	TCCR2B =((0<<CS22)|(1<<CS21)|(1<<CS20));

	/* Set TIMER2 clock asynchronus from external source, CTC mode */
	ASSR |= (1 << AS2);
	//TCCR2A =(1<<WGM21);
	// Prescale by  32 - 1024 ticks/sec, 250 msec max sleep
	TCCR2A = _BV(WGM21);
	TCCR2B =((0<<CS22)|(1<<CS21)|(1<<CS20));
	OCR2A = howlong-1;

	/* Reset timer count, wait for the write (which assures TCCR2x and OCR2A are finished) */
	TCNT2 = 0;
	while(ASSR & (1 << TCN2UB));

	/* Enable TIMER2 output compare interrupt, sleep mode and sleep */
	TIMSK2 |= (1 << OCIE2A);
	SMCR |= (1 <<  SE);
	sei();
	/* Ensure no activity is suspending the low power mode
	   before going to sleep. (edited by smeshlink) */
	if (OCR2A) sleep_mode();
	//...zzZZZzz...Ding!//

	/* Disable sleep mode after wakeup, so random code cant trigger sleep */
	SMCR  &= ~(1 << SE);
#if AVR_CONF_USE32KCRYSTAL
	/* Restore clock.c configuration */
	cli();
	TCCR2A = savedTCCR2A;
	TCCR2B = savedTCCR2B;
	OCR2A  = savedOCR2A;
	TCNT2  = savedTCNT2;
	sei();
#else
/* Disable TIMER2 interrupt */
    TIMSK2 &= ~(1 << OCIE2A);
#endif
	clock_adjust_ms(howlong);
	watchdog_start();
}
ISR(AVR_OUTPUT_COMPARE_INT)
{
	count++;
	milliseconds=1024 / CLOCK_SECOND+milliseconds;
	while(milliseconds >= 1024) {
		milliseconds -= 1024;
		seconds++;
	}
	/*  gcc will save all registers on the stack if an external routine is called */
	if(etimer_pending()) {
		etimer_request_poll();
	}

}
#if !AVR_CONF_USE32KCRYSTAL
/*---------------------------------------------------------------------------*/
/* TIMER2 Interrupt service */

ISR(TIMER2_COMPA_vect)
{

}
#endif /* !AVR_CONF_USE32KCRYSTAL */

/*---------------------------------------------------------------------------*/


