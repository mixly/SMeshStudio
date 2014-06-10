/* Copyright (c) 2007 Axel Wachtler
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
 * @brief Atmel Radio Development Kit, AT86RF230 Radio Adapter with Atmega1281.
 *
 *
 * This board wiring fits the Atmel radio development kit and/ or Dresden Elektronil RCBxxx family.
 * hardware and compatibles.
 *
 * The wiring of the radio and the ATmega is shown below:
 *
<pre>
          AVR      AT128RFA1
          ---      ---------
          PG0  -->  SLPTR
          PD4  <--  IRQ (ICP1)
          PG1  -->  RSTN
          PB0  -->  /SEL
          PB2  -->  MOSI
          PB3  <--  MISO
          PB1  -->  SCK

    LEDS PD4:PD5:PD6

Fuses/Locks:
     LF: 0xe2 - 16MHz TRANS RC Osc.
     HF: 0xDA - without boot loader
     EF: 0xff
     LOCK: 0xef - protection of boot section

Bootloader:
    Start at byte=0x1e000, address=0xf000, size = 4096 instructions/ 8192 bytes

</pre>

 @image html atmel_stk50x.jpg "STK500 + STK501 + Radio board"
 @image latex atmel_stk50x.jpg "STK500 + STK501 + Radio board"


@par Build Options

 All rdk2xx derivates will also work with a combination
 of STK500 + STK501 with mega1281 + Radio Extender Board
 on Expand1 connector.

 For UART connect PD2/PD3 on 10 pin header with RXD/TXD
 at STK501.

  - rdk230  : RCB230 prior V3.2 / V3.3.1 (AT86RF230A)
  - rdk230b : RCB230 V3.2 / RCB230 V3.3.1 (AT86RF230B)
  - rdk231  : RCB231 V4.0.2 / RCB231ED V4.1.1
  - rdk212  : RCB212SMA V5.3.2
  - rdk232  : Radio Extender Board + STK500 + STK501

 */



#ifndef BOARD_IDUINORF212_H
#define BOARD_IDUINORF212_H
#if defined(iduinoprorf212)
# define BOARD_TYPE BOARD_IDUINOPRORF212
# define BOARD_NAME "iduinoprorf212"
# define RADIO_TYPE (RADIO_AT86RF212)
#endif

/*=== Compile time parameters ========================================*/



#if BOARD_TYPE == BOARD_IDUINOPRORF212
# define LED_PORT      PORTD
# define LED_DDR       DDRD
# define LED_MASK      (0xE0)
# define LED_SHIFT     (5)
# define LEDS_INVERSE  (0)
# define LED_NUMBER    (3)
#endif
#define NO_KEYS        (1)



/*=== Host Interface ================================================*/
#define HIF_TYPE (HIF_UART_0)

#undef TRX_IF_RFA1
#undef TRX_IF_RFR2

#ifndef DEFAULT_SPI_RATE
# define DEFAULT_SPI_RATE  (SPI_RATE_1_16)
#endif

/*=== Hardware Components ============================================*/
#ifndef RADIO_TYPE
# define RADIO_TYPE (RADIO_AT86RF212)    /**< used radiio (see const.h)*/
#endif

/*=== TRX pin access macros ==========================================*/

#define DDR_TRX_RESET   DDRG      /**< DDR register for RESET pin */
#define PORT_TRX_RESET  PORTG         /**< PORT register for RESET pin */
#define MASK_TRX_RESET  (_BV(1))      /**< PIN mask for RESET pin */

#define TRX_RESET_INIT() DDR_TRX_RESET |= MASK_TRX_RESET    /**< RESET pin IO initialization */
#define TRX_RESET_HIGH() PORT_TRX_RESET |= MASK_TRX_RESET   /**< set RESET pin to high level */
#define TRX_RESET_LOW()  PORT_TRX_RESET &= ~MASK_TRX_RESET  /**< set RESET pin to low level */

#define PORT_TRX_SLPTR  PORTG         /**< DDR register for SLP_TR pin */
#define DDR_TRX_SLPTR   DDRG          /**< PORT register for SLP_TR pin */
#define MASK_TRX_SLPTR  (_BV(0))    /**< PIN mask for SLP_TR pin */

/** SLP_TR pin IO initialization */
#define TRX_SLPTR_INIT() DDR_TRX_SLPTR |= MASK_TRX_SLPTR
/** set SLP_TR pin to high level */
#define TRX_SLPTR_HIGH() PORT_TRX_SLPTR |= MASK_TRX_SLPTR
/**< set SLP_TR pin to low level */
#define TRX_SLPTR_LOW()  PORT_TRX_SLPTR &= ~MASK_TRX_SLPTR


/*=== IRQ access macros ==============================================*/
# define TRX_IRQ_vect    TIMER1_CAPT_vect    /**< interrupt vector name */

/** init interrupt handling
 *  - rising edge triggers ICP1 (ICES1),
 *  - timer capture is enabled (ICF1)
 */
# define TRX_IRQ_INIT()  do{\
                            /* TCCR1B |= (_BV(ICNC1) | _BV(ICES1) | _BV(CS12) | _BV(CS10)); */\
                            TCCR1B |= (_BV(ICNC1) | _BV(ICES1));\
                            TIFR1 = _BV(ICF1);\
                          } while(0)

/** disable TRX interrupt */
#define DI_TRX_IRQ() {TIMSK1 &= ~_BV(ICIE1);}

/** enable TRX interrupt */
#define EI_TRX_IRQ() {TIMSK1 |= _BV(ICIE1);}

/** timestamp register for RX_START event */
#define TRX_TSTAMP_REG ICR1


/*=== SPI access macros ==============================================*/
#define DDR_SPI  (DDRB)   /**< DDR register for SPI port */
#define PORT_SPI (PORTB)  /**< PORT register for SPI port */

#define SPI_MOSI _BV(PB2)  /**< PIN mask for MOSI pin */
#define SPI_MISO _BV(PB3)  /**< PIN mask for MISO pin */
#define SPI_SCK  _BV(PB1)  /**< PIN mask for SCK pin */
#define SPI_SS   _BV(PB0)  /**< PIN mask for SS pin */

#define SPI_DATA_REG SPDR  /**< abstraction for SPI data register */


/**
 * @brief inline function for SPI initialization
 */
static inline void SPI_INIT(uint8_t spirate)
{
    /* first configure SPI Port, then SPCR */
    PORT_SPI |= SPI_SCK | SPI_SS;
    DDR_SPI  |= SPI_MOSI | SPI_SCK | SPI_SS;
    DDR_SPI  &= ~SPI_MISO;

    SPCR = (_BV(SPE) | _BV(MSTR));

    SPCR &= ~(_BV(SPR1) | _BV(SPR0) );
    SPSR &= ~_BV(SPI2X);

    SPCR |= (spirate & 0x03);
    SPSR |= ((spirate >> 2) & 0x01);

}

/** set SS line to low level */
#define SPI_SELN_LOW()       uint8_t sreg = SREG; cli(); PORT_SPI &=~SPI_SS
/** set SS line to high level */
#define SPI_SELN_HIGH()      PORT_SPI |= SPI_SS; SREG = sreg
/** wait until SPI transfer is ready */
#define SPI_WAITFOR()        do { while((SPSR & _BV(SPIF)) == 0);} while(0)
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

#endif /* BOARD*/
