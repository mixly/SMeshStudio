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

/** Definition of Port Pin for temp sensor and light sensor. */
#define TEMP_DDR DDRD
#define TEMP_PORT PORTD
#define TEMP_BIT PORTD5
#define LIGHT_DDR DDRD
#define LIGHT_PORT PORTD
#define LIGHT_BIT PORTD4

/** Type used together with temp_find() to find temperature */
typedef enum {
    TEMP_ZERO_OFFSET_CELCIUS    = -15,
    TEMP_ZERO_OFFSET_FAHRENHEIT =   0
} temp_zero_offset_t;

/** Celcius temperatures (ADC-value) from -15 to 60 degrees */
static const uint16_t temp_table_celcius[] PROGMEM = {
    923,917,911,904,898,891,883,876,868,860,851,843,834,825,815,
    806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
    636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
    452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
    297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
    187,
};

/** Fahrenheit temperatures (ADC-value) from 0 to 140 degrees */
static const uint16_t temp_table_fahrenheit[] PROGMEM = {
    938, 935, 932, 929, 926, 923, 920, 916, 913, 909, 906, 902, 898,
    894, 891, 887, 882, 878, 874, 870, 865, 861, 856, 851, 847, 842,
    837, 832, 827, 822, 816, 811, 806, 800, 795, 789, 783, 778, 772,
    766, 760, 754, 748, 742, 735, 729, 723, 716, 710, 703, 697, 690,
    684, 677, 670, 663, 657, 650, 643, 636, 629, 622, 616, 609, 602,
    595, 588, 581, 574, 567, 560, 553, 546, 539, 533, 526, 519, 512,
    505, 498, 492, 485, 478, 472, 465, 459, 452, 446, 439, 433, 426,
    420, 414, 408, 402, 396, 390, 384, 378, 372, 366, 360, 355, 349,
    344, 338, 333, 327, 322, 317, 312, 307, 302, 297, 292, 287, 282,
    277, 273, 268, 264, 259, 255, 251, 246, 242, 238, 234, 230, 226,
    222, 219, 215, 211, 207, 204, 200, 197, 194, 190, 187,
};

static uint8_t temp_initialized = 0;
static uint8_t light_initialized = 0;

static void
temp_init()
{
  LIGHT_DDR &= ~(1 << LIGHT_BIT);
  TEMP_DDR |= (1 << TEMP_BIT);
  temp_initialized = 1;
  light_initialized = 0;
}

static void
light_init()
{
  TEMP_DDR &= ~(1 << TEMP_BIT);
  LIGHT_DDR |= (1 << LIGHT_BIT);
  temp_initialized = 0;
  light_initialized = 1;
}

/**
 *   \brief Find array index corresponding to input ADC value
 *
 *   Returned array index is actual temperature + zero offset. To
 *   get actual temperature, the zero offset (\ref temp_zero_offset_t)
 *   has to be subtracted.
 *
 *   \param[in] value Value to search for in table
 *   \param[in] array Pointer to array in which to look for ADC value
 *   \param[in] count Size of array
 *
 *   \return         EOF on error
*/
static int
temp_find(int16_t value, uint16_t* array, int count)
{
  int i = 0;
  int table_val = 0;
  do {
    table_val = pgm_read_word(&array[i]);
    if (table_val < value) {
      return i;
    }
  } while(++i<count);
  return EOF;
}

static float
temp_findf(int16_t value, uint16_t* array, int count)
{
  int i = 0;
  int table_val = 0;
  do {
    table_val = pgm_read_word(&array[i]);
    if (table_val < value) {
      if (i > 0) {
        int table_prev_val = pgm_read_word(&array[i - 1]);
        return (float)(value - table_val) / (float)(table_prev_val - table_val);
      }
      else {
        return i;
      }
    }
  } while(++i<count);
  return EOF;
}

/** \brief          Read current temperature's raw value
 * \return          EOF on error
 */
int16_t
sensor_temp_get_raw()
{
  int16_t result;
  if (!temp_initialized) {
    temp_init();
  }
  /* Power up sensor */
  TEMP_PORT |= (1 << TEMP_BIT);

  /* Init ADC and measure */
  adc_init(ADC_CHAN_ADC4, ADC_TRIG_FREE_RUN, ADC_REF_AVCC, ADC_PS_128);
  adc_conversion_start();
  while ((result = adc_result_get(ADC_ADJ_RIGHT)) == EOF ){
    ;
  }
  adc_deinit();

  /* Power down sensor */
  TEMP_PORT &= ~(1 << TEMP_BIT);

  return result;
}

/** \brief          Read current temperature
 * \param[in] unit  Selected temperature unit (\ref temp_unit_t)
 * \return          EOF on error
 */
int16_t
sensor_temp_get(temp_unit_t unit)
{
  int16_t res = sensor_temp_get_raw();
  int16_t temp;
  if (unit == TEMP_UNIT_CELCIUS) {
    temp = temp_find(res, temp_table_celcius, sizeof(temp_table_celcius)/sizeof(int)) + TEMP_ZERO_OFFSET_CELCIUS;
  } else /*unit == TEMP_UNIT_FAHRENHEIT*/{
    temp = temp_find(res, temp_table_fahrenheit, sizeof(temp_table_fahrenheit)/sizeof(int)) + TEMP_ZERO_OFFSET_FAHRENHEIT;
  }
  return temp;
}

float
sensor_temp_getf(temp_unit_t unit)
{
  int16_t res = sensor_temp_get_raw();
  float temp;
  if (unit == TEMP_UNIT_CELCIUS) {
    temp = temp_findf(res, temp_table_celcius, sizeof(temp_table_celcius)/sizeof(int)) + TEMP_ZERO_OFFSET_CELCIUS;
  } else /*unit == TEMP_UNIT_FAHRENHEIT*/{
    temp = temp_findf(res, temp_table_fahrenheit, sizeof(temp_table_fahrenheit)/sizeof(int)) + TEMP_ZERO_OFFSET_FAHRENHEIT;
  }
  return temp;
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
  adc_init(ADC_CHAN_ADC4, ADC_TRIG_FREE_RUN, ADC_REF_AVCC, ADC_PS_128);
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

/** \brief          Read current voltage
 * \return          EOF on error
 */
double
voltage_get() {
  int16_t result;
  /* Init ADC and measure */
  adc_init(ADC_CHAN_ADC30, ADC_TRIG_FREE_RUN, ADC_REF_AVCC, ADC_PS_128);
  adc_conversion_start();
  while ((result = adc_result_get(ADC_ADJ_RIGHT)) == EOF ){
    ;
  }
  adc_deinit();
  return 1.1 * (1024.0 / (double)result);
}
