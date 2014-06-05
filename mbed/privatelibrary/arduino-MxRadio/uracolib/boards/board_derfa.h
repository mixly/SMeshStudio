/* Copyright (c) 2009 - 2011 Axel Wachtler
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of the authors nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE. */

/* $Id$ */
/**
 * @file
 * @brief Dresden Elektronik Radio Module deRFmega128-22A001
 *
 *
 *
<pre>

Fuses/Locks:
     LF: 0xe2 - 8MHz internal RC Osc.
     HF: 0x11 - without boot loader
     HF: 0x10 - with boot loader
     EF: 0xff
     LOCK: 0xef - protection of boot section

Bootloader:
    Start at byte=0x1e000, address=0xf000, size = 4096 instructions/ 8192 bytes


radiofaro:
  LEDS: PG1, PG2

</pre>


@par Build Options

  - derfa1 : Radio Module deRFmega128-22A001

  - radiofaro : wireless Ardino board
  - radiofaro_v1 : wireless Ardino board

  - zigduino : Logos Electromechanical LLC
               http://www.logos-electro.com/blog/2010/5/16/zigduino.html
 */

#ifndef BOARD_DERFA_H
#define BOARD_DERFA_H

#if defined(derfa1)
# define BOARD_TYPE BOARD_DERFA1
# define BOARD_NAME "derfa1"
#elif defined(radiofaro)
# define BOARD_TYPE BOARD_RADIOFARO
# define BOARD_NAME "radiofaro"
#elif defined(radiofaro_v1)
# define BOARD_TYPE BOARD_RADIOFARO_V1
# define BOARD_NAME "radiofaro v1"
#elif defined(zigduino)
# define BOARD_TYPE BOARD_ZIGDUINO
# define BOARD_NAME "zigduino"
#elif defined(xxo)
# define BOARD_TYPE BOARD_XXO
# define BOARD_NAME "tic_tac_toe"
#elif defined(iduinorfa1)
# define BOARD_TYPE BOARD_IDUINORFA1
# define BOARD_NAME "iduinorfa1"
#endif
/*=== Compile time parameters ========================================*/

/*=== Hardware Components ============================================*/
#define RADIO_TYPE (RADIO_ATMEGA128RFA1_C)

/** timestamp register for RX_START event */
#define TRX_TSTAMP_REG TCNT1
#if BOARD_TYPE == BOARD_RADIOFARO
# define LED_PORT      PORTG
# define LED_DDR       DDRG
# define LED_MASK      (0x06)
# define LED_SHIFT     (1)
# define LEDS_INVERSE  (0)
# define LED_NUMBER    (2)
#elif BOARD_TYPE == BOARD_XXO

/* We just define the first three LEDs in row 1 
 * use a special display driver, that is frequently 
 * updated to handle the full LED matrix.
 */
 
#define LED_INIT() \
    do {\
        DDRB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3));\
        PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3));\
    } while(0)


#define LED_SET(x) \
  switch (x) { \
  case 0: PORTB &= ~_BV(0); break; \
  case 1: PORTB &= ~_BV(1); break; \
  case 2: PORTB &= ~_BV(2); break; \
  }

#define LED_CLR(x) \
  switch (x) { \
  case 0: PORTB |= _BV(0); break; \
  case 1: PORTB |= _BV(1); break; \
  case 2: PORTB |= _BV(2); break; \
  }

#define LED_SET_VALUE(x) \
  do { \
  if (x & 1) PORTB &= ~_BV(0); else PORTB |= _BV(0); \
  if (x & 2) PORTB &= ~_BV(1); else PORTB |= _BV(1); \
  if (x & 4) PORTB &= ~_BV(2); else PORTB |= _BV(2); \
  } while (0)

#define LED_GET_VALUE() ( \
  ((PORTB & _BV(0))? 0: 1) | \
  ((PORTB & _BV(1))? 0: 2) | \
  ((PORTB & _BV(2))? 0: 4) \
			  )

#define LED_VAL(msk,val) do{}while(0) /**< \todo how to implement this? */

#define LED_TOGGLE(ln) \
  switch (ln) { \
  case 0: PORTB ^= _BV(5); break; \
  case 1: PORTB ^= _BV(7); break; \
  case 2: PORTB ^= _BV(6); break; \
  }

#define LED_NUMBER (3)
#define LED_ACTIVITY (0)

#else
# define LED_NUMBER    (0)
# define NO_LEDS       (1)
#endif
#define NO_KEYS        (1)

/*=== Host Interface ================================================*/
#if BOARD_TYPE == BOARD_RADIOFARO_V1 || BOARD_TYPE == BOARD_ZIGDUINO || BOARD_TYPE == BOARD_IDUINORFA1
# define HIF_TYPE    HIF_UART_0
#else
# define HIF_TYPE    HIF_UART_1
#endif

#define TRX_RESET_LOW()   do { TRXPR &= ~_BV(TRXRST); } while (0)
#define TRX_RESET_HIGH()  do { TRXPR |= _BV(TRXRST); } while (0)
#define TRX_SLPTR_LOW()   do { TRXPR &= ~_BV(SLPTR); } while (0)
#define TRX_SLPTR_HIGH()  do { TRXPR |= _BV(SLPTR); } while (0)

/*=== TIMER Interface ===============================================*/
#define HWTMR_PRESCALE  (1)
#define HWTIMER_TICK    ((1.0*HWTMR_PRESCALE)/F_CPU)
#define HWTIMER_TICK_NB (0xFFFFUL)
#define HWTIMER_REG     (TCNT1)
#define TIMER_TICK      (HWTIMER_TICK_NB * HWTIMER_TICK)
#define TIMER_POOL_SIZE (4)
#define TIMER_INIT() \
    do{ \
        TCCR1B |= (_BV(CS10)); \
        TIMSK1 |= _BV(TOIE1); \
    }while(0)
#define TIMER_IRQ_vect   TIMER1_OVF_vect

#endif /* BOARD_DERFA_H */
