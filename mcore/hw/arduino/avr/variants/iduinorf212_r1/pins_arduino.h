/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

/*
	This version of pins_arduino.h is for the iduino r1
	SMeshLink 2012 Aug 10
*/


#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#ifndef iduinorf212
#define iduinorf212
#endif
#define CHINABAND
#include <avr/pgmspace.h>
#define NUM_DIGITAL_PINS 38
#define NUM_ANALOG_INPUTS 8
#define analogInputToDigitalPin(p) (((p) < 6) ? (p) + 14 : \
		((p) == 6) ? 33 : \
				((p) == 7) ? 35 :-1)
#define digitalPinHasPWM(p) ((p) == 3 ||(p) == 4 ||(p) == 6 ||(p) == 8 ||(p) == 9 ||(p) == 27)

const static uint8_t SS = 10;
const static uint8_t MOSI = 11;
const static uint8_t MISO = 12;
const static uint8_t SCK = 13;

const static uint8_t SDA = 21;
const static uint8_t SCL = 22;
const static uint8_t LED = 13;

const static uint8_t A0 = 14;
const static uint8_t A1 = 15;
const static uint8_t A2 = 16;
const static uint8_t A3 = 17;
const static uint8_t A4 = 18;
const static uint8_t A5 = 19;
const static uint8_t A6 = 33;
const static uint8_t A7 = 35;

const static uint8_t RFTX = 23;
const static uint8_t RFRX = 24;
const static uint8_t LED1 = 23;
const static uint8_t LED2 = 24;

const static uint8_t LED3 = 25;

//const static uint8_t BATMON = 35;

// A majority of the pins are NOT PCINTs, SO BE WARNED (i.e. you cannot use them as receive pins)
// Only pins available for RECEIVE (TRANSMIT can be on any pin):
// Pins: 7, 8, 9, 10, 11, 12, 13, 20

#define digitalPinToPCICR(p) ( ((((p) >= 8) && ((p) <= 13)) || \
		((p) == 20) || ((p) == 36) || ((p) == 26)) ? \
				(&PCICR) : ((uint8_t *)0) )

#define digitalPinToPCICRbit(p) ( ((p) == 36) ? 1 : 0 )

#define digitalPinToPCMSK(p) ( ((((p) >= 8) && ((p) <= 13)) || ((p) == 20) || ((p) == 26)) ? (&PCMSK0) : \
		( ((p) == 36) ? (&PCMSK1) : \
				((uint8_t *)0) ) )

#define digitalPinToPCMSKbit(p) ( ((p) == 8) ? 4 : \
		( ((p) == 9) ? 7 : \
				( ((p) == 10) ? 6 : \
						( ((p) == 11) ? 5 : \
								( ((p) == 12) ? 3 : \
										( ((p) == 13) ? 1 : \
												( ((p) == 20) ? 2 : \
														( ((p) == 26) ? 0 : \
																( ((p) == 36) ? 0 : \
																		0 ) ) ) ) ) ) ) ))

#ifdef ARDUINO_MAIN

const uint16_t PROGMEM port_to_mode_PGM[] = {
		NOT_A_PORT,
		NOT_A_PORT,
		(uint16_t)&DDRB,
		NOT_A_PORT,
		(uint16_t)&DDRD,
		(uint16_t)&DDRE,
		(uint16_t)&DDRF,
		(uint16_t)&DDRG,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
		NOT_A_PORT,
		NOT_A_PORT,
		(uint16_t)&PORTB,
		NOT_A_PORT,
		(uint16_t)&PORTD,
		(uint16_t)&PORTE,
		(uint16_t)&PORTF,
		(uint16_t)&PORTG,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
		NOT_A_PORT,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
		NOT_A_PIN,
		NOT_A_PIN,
		(uint16_t)&PINB,
		NOT_A_PIN,
		(uint16_t)&PIND,
		(uint16_t)&PINE,
		(uint16_t)&PINF,
		(uint16_t)&PING,
		NOT_A_PIN,
		NOT_A_PIN,
		NOT_A_PIN,
		NOT_A_PIN,
		NOT_A_PIN,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
		// PORTLIST
		// -------------------------------------------
		PD	, // PD 2 ** 0 ** USART1_RX
		PD	, // PD 3 ** 1 ** USART1_TX
		PE	, // PE 6 ** 2 ** D2
		PE	, // PE 5 ** 3 ** PWM3
		PE	, // PE 2 ** 4 ** PWM4
		PE	, // PE 3 ** 5 ** D5
		PE	, // PE 4 ** 6 ** PWM6
		PE	, // PE 7 ** 7 ** D7
		PB	, // PB 4 ** 8 ** PWM8
		PB	, // PB 7 ** 9 ** PWM9
		PB	, // PB 0 ** 10 ** SS
		PB	, // PB 2 ** 11 ** SPI_MOSI
		PB	, // PB 3 ** 12 ** SPI_MISO
		PB	, // PB 1 ** 13 ** SPI_SCK
		PF	, // PF 0 ** 14 ** A0
		PF	, // PF 1 ** 15 ** A1
		PF	, // PF 2 ** 16 ** A2
		PF	, // PF 3 ** 17 ** A3
		PF	, // PF 4 ** 18 ** A4
		PF	, // PF 5 ** 19 ** A5

		PB	, // PB 5 ** 20 ** LIS3DH INT(POWER CONNECTED VCC)
		PD	, // PD 1 ** 21 ** I2C_SDA
		PD	, // PD 0 ** 22 ** I2C_SCL
		PD , // PD 5 ** 23 ** RFTX  LED1
		PD , // PD 6 ** 24 ** RFRX  LED2
		PD	, // PD 7 ** 25 ** 	    LED3
		PB , // PB 6 ** 26 **  DS2411
		PG	, // PG 5 ** 27 ** VCC_SD_CONTROL LOW ENABLE
		PD	, // PD 4 ** 28 ** RF212_IRQ
		PG	, // PG 3 ** 29 ** TOSC2 32.768KRC
		PG	, // PG 4 ** 30 ** TOSC1 32.768KRC
		PG	, // PG 1 ** 31 ** RF212_RST
		PG	, // PG 2 ** 32 **  SPI_CS_SD SI7005 POWER
		PF	, // PF 6 ** 33 ** VCC_ENC_CONTROL LOW ENABLE
		PG	, // PG 0 ** 34 ** RF212_SLP
		PF , // PF 7 ** 35 ** SPI_CS_ENC
		PE	, // PE 0 ** 36 ** USART0_RX CP2104
		PE	, // PE 1 ** 37 ** USART0_TX CP2104

};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
		// PIN IN PORT
		// -------------------------------------------
		_BV( 2 )	, // PD 0 ** 0 ** USART1_RX
		_BV( 3 )	, // PE 1 ** 1 ** USART1_TX
		_BV( 6 )	, // PE 6 ** 2 ** D2
		_BV( 5 )	, // PE 5 ** 3 ** PWM3
		_BV( 2 )	, // PE 2 ** 4 ** PWM4
		_BV( 3 )	, // PE 3 ** 5 ** D5
		_BV( 4 )	, // PE 4 ** 6 ** PWM6
		_BV( 7 )	, // PE 7 ** 7 ** D7
		_BV( 4 )	, // PB 4 ** 8 ** PWM8
		_BV( 7 )	, // PB 7 ** 9 ** PWM9
		_BV( 0 )	, // PB 0 ** 10 ** SS
		_BV( 2 )	, // PB 2 ** 11 ** SPI_MOSI
		_BV( 3 )	, // PB 3 ** 12 ** SPI_MISO
		_BV( 1 )	, // PB 1 ** 13 ** SPI_SCK
		_BV( 0 )	, // PF 0 ** 14 ** A0
		_BV( 1 )	, // PF 1 ** 15 ** A1
		_BV( 2 )	, // PF 2 ** 16 ** A2
		_BV( 3 )	, // PF 3 ** 17 ** A3
		_BV( 4 )	, // PF 4 ** 18 ** A4
		_BV( 5 )	, // PF 5 ** 19 ** A5
		_BV( 5 )	, // PB 5 ** 20 ** SPI_MOSI
		_BV( 1 )	, // PD 0 ** 21 ** I2C_SDA
		_BV( 0 )	, // PD 1 ** 22 ** I2C_SCL
		_BV( 5 ) , // PD 5 ** 23
		_BV( 6 ) , // PD 6 ** 24
		_BV( 7 )	, // PD 7 ** 25
		_BV( 6 ) , // PB 6 ** 26
		_BV( 5 )	, // PG 5 ** 27
		_BV( 4 )	, // PD 4 ** 28
		_BV( 3 )	, // PG 3 ** 29
		_BV( 4 )	, // PG 4 ** 30
		_BV( 1 )	, // PG 1 ** 31
		_BV( 2 )	, // PG 2 ** 32
		_BV( 6 )	, // PF 6 ** 33
		_BV( 0 )	, // PG 0 ** 34
		_BV( 7 ) , // PF 7 ** 35
		_BV( 0 )	, // PE 0 ** 36
		_BV( 1 )	, // PE 1 ** 37

};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
		// TIMERS
		// -------------------------------------------
		NOT_ON_TIMER	, // PE 0 ** 0 ** USART0_RX
		NOT_ON_TIMER	, // PE 1 ** 1 ** USART0_TX
		NOT_ON_TIMER	, // PE 2 ** 2 ** D2
		TIMER3C	, // PE 3 ** 3 ** PWM3
		NOT_ON_TIMER	, // PE 4 ** 4 ** D4
		TIMER3A	, // PE 6 ** 5 ** PWM5
		TIMER3B	, // PE 5 ** 6 ** PWM6
		NOT_ON_TIMER	, // PE 7 ** 7 ** D7
		TIMER2A	, // PB 4 ** 8 ** PWM8
		TIMER1C	, // PB 7 ** 9 ** PWM9
		NOT_ON_TIMER	, // PB 0 ** 10 ** SS
		NOT_ON_TIMER	, // PB 2 ** 11 ** SPI_MOSI
		NOT_ON_TIMER	, // PB 3 ** 12 ** SPI_MISO
		NOT_ON_TIMER	, // PB 1 ** 13 ** SPI_SCK
		NOT_ON_TIMER	, // PF 0 ** 14 ** A0
		NOT_ON_TIMER	, // PF 1 ** 15 ** A1
		NOT_ON_TIMER	, // PF 2 ** 16 ** A2
		NOT_ON_TIMER	, // PF 3 ** 17 ** A3
		NOT_ON_TIMER	, // PF 4 ** 18 ** A4
		NOT_ON_TIMER	, // PF 5 ** 19 ** A5
		NOT_ON_TIMER	, // PB 5 ** 20 **
		NOT_ON_TIMER	, // PD 1 ** 21 ** I2C_SDA
		NOT_ON_TIMER	, // PD 0 ** 22 ** I2C_SCL
		NOT_ON_TIMER , 		// PD 5 ** 23
		NOT_ON_TIMER , 		// PD 6 ** 24
		NOT_ON_TIMER	, // PD 7 ** 25
		NOT_ON_TIMER , 	// PB 6 ** 26
		TIMER0B	, 		// PG 5 ** 27
		NOT_ON_TIMER	, // PD 4 ** 28
		NOT_ON_TIMER	, // PG 3 ** 29
		NOT_ON_TIMER	, // PG 4 ** 30
		NOT_ON_TIMER	, // PG 1 ** 31
		NOT_ON_TIMER	, // PG 2 ** 32
		NOT_ON_TIMER	, // PF 6 ** 33
		NOT_ON_TIMER	, // PG 0 ** 34
		NOT_ON_TIMER , // PF 7 ** 35
		NOT_ON_TIMER	, // PE 0
		NOT_ON_TIMER	, // PE 1
};	

#endif

#endif

