/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: interrupt.c $
 */

/**
 * \file
 *         Interrupt architecture of the mx2xxcc port.
 * \author
 *         SmeshLink
 */

#include "contiki-conf.h"
#include "interrupt.h"

#include "sys/process.h"

static struct interrupt interrupts[INTERRUPT_NUM] = {
    { INT0, NULL },
    { INT1, NULL }
};
/*---------------------------------------------------------------------------*/
static void
delay50us(int t)
{
  unsigned char j;
  for(;t>0;t--)
    for(j=0;j<70;j++)
      asm volatile("nop");
}
/*---------------------------------------------------------------------------*/
void
interrupt_init1()
{
//  cbi(DDRD,2);
//  cbi(PORTD,2);

//  cbi(UCSR1B,TXEN1);
//  cbi(UCSR1B,RXEN1);
//  cbi(UCSR1B,TXCIE1);
//  cbi(UCSR1B,RXCIE1);

  /* Set INT0/INT1 to be raised by rising edge. */
  cbi(EICRA,ISC00);
  delay50us(1);
  sbi(EICRA,ISC01);
  delay50us(1);
  sbi(EICRA,ISC10);
  delay50us(1);
  sbi(EICRA,ISC11);
  delay50us(1);

  /* Enable INT0/INT1. */
  sbi(EIMSK, INT0);
  delay50us(1);
  sbi(EIMSK, INT1);
}
void
interrupt_init(int isc00, int isc01, int isc10, int isc11)
{
  if (isc00)
    sbi(EICRA, ISC00);
  else
    cbi(EICRA, ISC00);
  delay50us(1);

  if (isc01)
    sbi(EICRA, ISC01);
  else
    cbi(EICRA, ISC01);
  delay50us(1);

  if (isc10)
    sbi(EICRA, ISC10);
  else
    cbi(EICRA, ISC10);
  delay50us(1);

  if (isc11)
    sbi(EICRA, ISC11);
  else
    cbi(EICRA, ISC11);
  delay50us(1);
}
void
interrupt_enable(int int_vect)
{
  sbi(EIMSK, int_vect);
}
void
interrupt_disable(int int_vect)
{
  cbi(EIMSK, int_vect);
}
/*---------------------------------------------------------------------------*/
void
interrupt_register(int int_vect)
{
  int i = 0;
  for (; i < INTERRUPT_NUM; i++) {
    if (interrupts[i].int_vect == int_vect) {
      interrupts[i].p = PROCESS_CURRENT();
      break;
    }
  }
}
/*---------------------------------------------------------------------------*/
ISR(INT0_vect) {
  process_post(interrupts[INT0].p, PROCESS_EVENT_INTERRUPT, &interrupts[INT0]);
}
ISR(INT1_vect) {
  process_post(interrupts[INT1].p, PROCESS_EVENT_INTERRUPT, &interrupts[INT1]);
}
/*---------------------------------------------------------------------------*/
