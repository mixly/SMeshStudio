/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: adc-sensors.h $
 */

/**
 * \file
 *         ADC sensors API, defining a set of functions for accessing sensors.
 * \author
 *         SmeshLink
 */

#ifndef __ADC_SENSORS_H__
#define __ADC_SENSORS_H__

#include <stdio.h>

/** Type used with \ref sensor_temp_get() to select temperature unit */
typedef enum {
    TEMP_UNIT_CELCIUS,
    TEMP_UNIT_FAHRENHEIT
} temp_unit_t;


/** \brief          Read current temperature
 * \param[in] unit  Selected temperature unit (\ref temp_unit_t)
 * \return          EOF on error
 */
int16_t sensor_temp_get(temp_unit_t unit);

/** \brief          Read current voltage
 * \return          EOF on error
 */
double voltage_get();

#endif /* __ADC_SENSORS_H__ */
