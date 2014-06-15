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

/** Definition of Port Pin for temp sensor and light sensor. */
#define LIGHT_DDR DDRG
#define LIGHT_PORT PORTG
#define LIGHT_BIT PORTD1


uint8_t light_initialized = 0;
void
light_init()
{
  LIGHT_DDR |= (1 << LIGHT_BIT);
  light_initialized = 1;
}
void
light_shutdown()
{
  LIGHT_DDR &= ~(1 << LIGHT_BIT);;
  light_initialized = 0;
}
/** \brief          Read current light's raw value
 * \return          EOF on error
 */
int16_t
sensor_light_get_raw()
{
  int16_t result;
  if (!light_initialized) {
    light_init();
  }
  /* Power up sensor */
  LIGHT_PORT |= (1 << LIGHT_BIT);

  /* Init ADC and measure */
  adc_init(ADC_CHAN_ADC6, ADC_TRIG_FREE_RUN, ADC_REF_AVCC, ADC_PS_128);
  adc_conversion_start();
  while ((result = adc_result_get(ADC_ADJ_RIGHT)) == EOF ){
    ;
  }
  adc_deinit();

  /* Power down sensor */
  LIGHT_PORT &= ~(1 << LIGHT_BIT);

  return result;
}

/** \brief          Read current light
 * \return          EOF on error
 */
int16_t
sensor_light_get()
{
  int16_t res = sensor_light_get_raw();
  return res;
}
/** \brief          Read current temperature
 * \param[in] unit  Selected temperature unit (\ref temp_unit_t)
 * \return          EOF on error
 */
int16_t sensor_temp_get(temp_unit_t unit)
{
	int32_t reading = 0;
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
	return (double)(2550-75*16-75+75*i) /1000 ;

}
