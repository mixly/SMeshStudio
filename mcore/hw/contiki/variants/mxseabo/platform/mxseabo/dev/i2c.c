/*
 * Copyright (c) 2011-2013 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * i2c.c
 *
 *  Created on: Nov 27, 2013
 *      Author: smeshlink
 */

#include <avr/io.h>
#include "i2c.h"

#ifndef sbi
#define sbi(port, bit) ((port) |= _BV(bit))
#endif
#ifndef cbi
#define cbi(port, bit) ((port) &= ~_BV(bit))
#endif
#ifndef inp
#define inp(port) (port)
#endif

#if defined __AVR_ATmega1284P__
#define sck     0
#define sda     1
// hardware pin functions
void SET_CLOCK() {sbi(PORTC,sck);}
void CLEAR_CLOCK() {cbi(PORTC,sck);}
void MAKE_CLOCK_OUTPUT() {sbi(DDRC,sck);}
void MAKE_CLOCK_INPUT() {cbi(DDRC,sck);}

void SET_DATA() {sbi(PORTC,sda);}
void CLEAR_DATA() {cbi(PORTC,sda);}
void MAKE_DATA_OUTPUT() {sbi(DDRC,sda);}
void MAKE_DATA_INPUT() {cbi(DDRC,sda);}

char GET_DATA() {return (inp(PINC) & (1 << sda));}
#endif

#if defined __AVR_ATmega1281__
#define sck     0
#define sda     1
// hardware pin functions
void SET_CLOCK() {sbi(PORTD,sck);}
void CLEAR_CLOCK() {cbi(PORTD,sck);}
void MAKE_CLOCK_OUTPUT() {sbi(DDRD,sck);}
void MAKE_CLOCK_INPUT() {cbi(DDRD,sck);}

void SET_DATA() {sbi(PORTD,sda);}
void CLEAR_DATA() {cbi(PORTD,sda);}
void MAKE_DATA_OUTPUT() {sbi(DDRD,sda);}
void MAKE_DATA_INPUT() {cbi(DDRD,sda);}

char GET_DATA() {return (inp(PIND) & (1 << sda));}
#endif

void
i2c_delay_us(int t)
{
  for(;t>0;t--) {
    asm volatile("nop");
    asm volatile("nop");
  }
}

static void
pulse_clock()
{
  i2c_delay_us(100);
  SET_CLOCK();
  i2c_delay_us(100);
  CLEAR_CLOCK();
}

static unsigned char
read_bit()
{
  unsigned char i;

  MAKE_DATA_INPUT();
  i2c_delay_us(100);
  SET_CLOCK();
  i2c_delay_us(100);
  i = (unsigned char) GET_DATA();
  CLEAR_CLOCK();
  return i;
}

static unsigned char
i2c_read()
{
  unsigned char data = 0;
  unsigned char i = 0;
  for (i = 0; i < 8; i++) {
    data = (data << 1) & 0xfe;
    if (read_bit() == 1) {
      data |= 0x1;
    }
  }
  return data;
}

static unsigned char
i2c_write(unsigned char c)
{
  unsigned char i;
  MAKE_DATA_OUTPUT();
  for (i = 0; i < 8; i++) {
    if (c & 0x80) {
      SET_DATA();
    } else {
      CLEAR_DATA();
    }
    pulse_clock();
    c = c << 1;
  }
  i = read_bit();
  return i == 0;
}

void
i2c_init()
{
  SET_CLOCK();
  SET_DATA();
  MAKE_CLOCK_OUTPUT();
  MAKE_DATA_OUTPUT();
}

static void
i2c_start()
{
  SET_DATA();
  SET_CLOCK();
  MAKE_DATA_OUTPUT();
  i2c_delay_us(100);
  CLEAR_DATA();
  i2c_delay_us(100);
  CLEAR_CLOCK();
}

static void
i2c_ack()
{
  MAKE_DATA_OUTPUT();
  CLEAR_DATA();
  pulse_clock();
}

static void
i2c_nack()
{
  MAKE_DATA_OUTPUT();
  SET_DATA();
  pulse_clock();
}

static void
i2c_sendack(char ack)
{
  if (!ack)
    i2c_ack();
  else
    i2c_nack();
}

static void
i2c_end()
{
  MAKE_DATA_OUTPUT();
  CLEAR_DATA();
  i2c_delay_us(100);
  SET_CLOCK();
  i2c_delay_us(100);
  SET_DATA();
}

void
real_init_i2c()
{
  i2c_start();
  i2c_write(0x4e);
  i2c_write(0x03);
  i2c_write(0x00);
  i2c_end();
}

void
i2c_config(unsigned char value)
{
  i2c_start();
  i2c_write(0x4e);
  i2c_write(0x01);
  i2c_write(value);
  i2c_end();
}
