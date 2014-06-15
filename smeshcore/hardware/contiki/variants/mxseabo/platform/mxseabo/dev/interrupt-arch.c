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
    { PCINT24, NULL },
    { PCINT25, NULL },
    { PCINT26, NULL },
    { PCINT27, NULL },
    { PCINT28, NULL },
    { PCINT29, NULL },
    { PCINT30, NULL },
    { PCINT31, NULL }
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
  sbi(PCMSK3, int_vect);
  sbi(PCICR, PCIE3);
}
void
interrupt_disable(int int_vect)
{
  cbi(PCMSK3, int_vect);
  cbi(PCICR, PCIE3);
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
ISR(PCINT3_vect) {
  interrupt_disable(PCINT28);
  process_post(interrupts[PCINT28].p, PROCESS_EVENT_INTERRUPT, &interrupts[PCINT28]);
}
/*---------------------------------------------------------------------------*/
