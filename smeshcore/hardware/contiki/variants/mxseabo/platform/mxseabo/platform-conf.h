/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
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
 * $Id: platform-conf.h,v 1.1 2010/06/23 10:25:54 joxe Exp $
 */

/**
 * \file
 *         Platform configuration of the SmeshLink MX2XXCC port.
 * \author
 *         SmeshLink
 */

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__



/*
 * Definitions below are dictated by the hardware and not really
 * changeable!
 */
/* Platform name, type, and MCU clock rate */
#define PLATFORM_NAME  "MX2XXCC"
#define PLATFORM_TYPE  MX2XXCC
#ifndef F_CPU
#define F_CPU          8000000UL
#endif

#define AVR_CONF_TMR0_PRESCALE 1024

/* The AVR tick interrupt usually is done with an 8 bit counter around 128 Hz.
 * 125 Hz needs slightly more overhead during the interrupt, as does a 32 bit
 * clock_time_t.
 */
/* Clock ticks per second */
#define CLOCK_CONF_SECOND 128
#if 1
/* 16 bit counter overflows every ~10 minutes */
typedef unsigned short clock_time_t;
#define CLOCK_LT(a,b)  ((signed short)((a)-(b)) < 0)
#define INFINITE_TIME 0xffff
#define RIME_CONF_BROADCAST_ANNOUNCEMENT_MAX_TIME INFINITE_TIME/CLOCK_CONF_SECOND /* Default uses 600 */
#define COLLECT_CONF_BROADCAST_ANNOUNCEMENT_MAX_TIME INFINITE_TIME/CLOCK_CONF_SECOND /* Default uses 600 */
#else
typedef unsigned long clock_time_t;
#define CLOCK_LT(a,b)  ((signed long)((a)-(b)) < 0)
#define INFINITE_TIME 0xffffffff
#endif
/* These routines are not part of the contiki core but can be enabled in cpu/avr/clock.c */
void clock_delay_msec(uint16_t howlong);
void clock_adjust_ticks(clock_time_t howmany);

/* LED ports */
#define LEDS_PxDIR DDRA // port direction register
#define LEDS_PxOUT PORTA // port register
#define LEDS_CONF_RED    32 //red led
#define LEDS_CONF_GREEN  64 // green led
#define LEDS_CONF_YELLOW 128 // yellow led

/* COM port to be used for SLIP connection */
#define SLIP_PORT RS232_PORT_0

/* Pre-allocated memory for loadable modules heap space (in bytes)*/
#define MMEM_CONF_SIZE 256

/* Use the following address for code received via the codeprop
 * facility
 */
#define EEPROMFS_ADDR_CODEPROP 0x8000

#define EEPROM_NODE_ID_START 0x00
/*
 * SPI bus configuration for the TMote Sky.
 */

/* SPI input/output registers. */
#define SPI_TXBUF SPDR
#define SPI_RXBUF SPDR

#define BV(bitno) _BV(bitno)

#define SPI_WAITFOREOTx() do { while (!(SPSR & BV(SPIF))); } while (0)
#define SPI_WAITFOREORx() do { while (!(SPSR & BV(SPIF))); } while (0)

#define CONTIKI_SCK            1  /* - Output: SPI Serial Clock (SCLK) - ATMEGA128 PORTB, PIN1 */
#define CONTIKI_MOSI           2  /* - Output: SPI Master out - slave in (MOSI) - ATMEGA128 PORTB, PIN2 */
#define CONTIKI_MISO           3  /* - Input:  SPI Master in - slave out (MISO) - ATMEGA128 PORTB, PIN3 */



#define CSN            0

#endif /* __PLATFORM_CONF_H__ */
