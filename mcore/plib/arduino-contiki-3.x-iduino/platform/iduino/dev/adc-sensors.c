/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: adc-sensors.c $
 */

/**
 * \file
 *         ADC sensors API implementation for mx2xxcc.
 * \author
 *         SmeshLink
 */

#include <avr/pgmspace.h>

#include "adc-sensors.h"
#include "adc.h"
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

/** \brief          Read current temperature
 * \param[in] unit  Selected temperature unit (\ref temp_unit_t)
 * \return          EOF on error
 */
int16_t sensor_temp_get(temp_unit_t unit)
{
	int reading = 0;
	int result = 0;
	ADCSRB |= _BV(MUX5);
	ADMUX = _BV(REFS1) | _BV(REFS0) | 0b1001 ;
	ADCSRA = _BV(ADEN) | _BV(ADPS0) | _BV(ADPS2) ;
	sbi(ADCSRA,ADSC);
	loop_until_bit_is_clear(ADCSRA,ADSC);
	reading = ADC;
	ADCSRA=0;                 //disable ADC
	ADCSRB=0;                 //disable ADC
	ADMUX=0;                  //turn off internal vref
	if (unit == TEMP_UNIT_CELCIUS) {
		result = reading * 113 - 27280;
	} else /*unit == TEMP_UNIT_FAHRENHEIT*/{
		result = (reading * 113 - 27280)*9/5+3200;
	}
	return result;


}

/** \brief          Read current voltage
 * \return          EOF on error
 */
double
voltage_get() {
	BATMON = 16;
	int i;
	for ( i=16; i<31; i++)
	{
		BATMON = i;
		if ((BATMON &(1<<BATMON_OK))==0) break;
	}
	return ((double)(2550-75*16-75+75*i))/1000.0;

}
