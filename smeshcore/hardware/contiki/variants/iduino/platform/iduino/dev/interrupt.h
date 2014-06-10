/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: interrupt.h $
 */

/**
 * \file
 *         Interrupt interfaces.
 * \author
 *         SmeshLink
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <avr/interrupt.h>

#ifndef sbi
/* avr-libc 1.2.3 doesn't include these anymore. */
#define sbi(port, bit) ((port) |= _BV(bit))
#define cbi(port, bit) ((port) &= ~_BV(bit))
#define ybi(port, bit) ((port) ^= _BV(bit))
#define gbi(pin ,bit)   ( pin &   (1<<bit) )    //example: gbi(PINA,0);
#define inp(port) (port)
#define inb(port) (port)
#define outp(value, port) ((port) = (value))
#define outb(port, value) ((port) = (value))
#define inw(port) (*(volatile uint16_t *)&port)
#define PRG_RDB(addr) pgm_read_byte(addr)
#endif

/* Interrupt event */
#define PROCESS_EVENT_INTERRUPT 0x90
/* Number of interrupts */
#define INTERRUPT_NUM 8

struct interrupt {
  int int_vect;
  struct process *p;
};

/* Initialize interrupt handlers. */
void interrupt_init(int int_vect,int mode); //0 means falling 1 means rising
void interrupt_enable(int int_vect);
void interrupt_disable(int int_vect);
/* Register current process with an interrupt specified by int_vect. */
void interrupt_register(int int_vect);

#endif /* __INTERRUPT_H__ */
