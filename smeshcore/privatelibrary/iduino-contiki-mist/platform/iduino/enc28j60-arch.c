/*
 * Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <avr/io.h>
#include <util/delay.h>
#include <clock.h>
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_init(void)
{

	/* POWER ENC28J60,set  GPIO low */
	ENC28J60_POWER_DDR |= 1<<ENC28J60_POWER_CS;
	ENC28J60_POWER_PORT &= ~(1<<ENC28J60_POWER_CS);

	clock_delay(25);
	/* SPI SELECT ENC28J60,set  GPIO low */
	ENC28J60_CONTROL_DDR |= 1<<ENC28J60_CONTROL_CS;
	CSACTIVE;

	/* Set all pins to GPIO mode */
	DDRB  |= 1<<PB2 |1<<PB1 | 1<<PB0; // mosi, sck, ss output
	PORTB |= (1<<PB0);
	DDRB  &= ~(1<<PB3);// MISO is input

	/* The CS pin is active low, so we set it high when we haven't   selected the chip. */
	CSPASSIVE;
	PORTB  &= ~(1<<PB2);
	PORTB  &= ~(1<<PB1);
	// initialize SPI interface
	// master mode and Fosc/2 clock:
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR |= (1<<SPI2X);
	// perform system reset
}

