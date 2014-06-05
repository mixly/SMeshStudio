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

 struct interrupt interrupts[INTERRUPT_NUM] = {
		{ INT0, NULL },
		{ INT1, NULL },
		{ INT2, NULL },
		{ INT3, NULL },
		{ INT4, NULL },
		{ INT5, NULL },
		{ INT6, NULL },
		{ INT7, NULL }

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

void
interrupt_init(int int_vect,int mode)
{
	switch ( int_vect )
	{
	case  INT0:
		if (mode==0) {
			sbi(EICRA, ISC01);
			cbi(EICRA, ISC00);
		}
		else {
			sbi(EICRA, ISC01);
			sbi(EICRA, ISC00);
		}
		delay50us(1);
		break;
	case  INT1:
		if (mode==0) {
			sbi(EICRA, ISC11);
			cbi(EICRA, ISC10);
		}
		else {
			sbi(EICRA, ISC01);
			sbi(EICRA, ISC00);
		}
		delay50us(1);
		break;
	case  INT2:
		if (mode==0) {
			sbi(EICRA, ISC21);
			cbi(EICRA, ISC20);
		}
		else {
			sbi(EICRA, ISC21);
			sbi(EICRA, ISC20);
		}
		delay50us(1);
		break;
	case  INT3:
		if (mode==0) {
			sbi(EICRA, ISC31);
			cbi(EICRA, ISC30);
		}
		else {
			sbi(EICRA, ISC31);
			sbi(EICRA, ISC30);
		}
		delay50us(1);
		break;
	case  INT4:
		if (mode==0) {
			sbi(EICRB, ISC41);
			cbi(EICRB, ISC40);
		}
		else {
			sbi(EICRB, ISC41);
			sbi(EICRB, ISC40);
		}
		delay50us(1);
		break;
	case  INT5:
		if (mode==0) {
			sbi(EICRB, ISC51);
			cbi(EICRB, ISC50);
		}
		else {
			sbi(EICRB, ISC51);
			sbi(EICRB, ISC50);
		}
		delay50us(1);

		break;
	case  INT6:
		if (mode==0) {
			sbi(EICRB, ISC61);
			cbi(EICRB, ISC60);
		}
		else {
			sbi(EICRB, ISC61);
			sbi(EICRB, ISC60);
		}
		delay50us(1);

		break;
	case  INT7:
		if (mode==0) {
			sbi(EICRB, ISC71);
			cbi(EICRB, ISC70);
		}
		else {
			sbi(EICRB, ISC71);
			sbi(EICRB, ISC70);
		}
		delay50us(1);

		break;
	}

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

/*---------------------------------------------------------------------------*/
