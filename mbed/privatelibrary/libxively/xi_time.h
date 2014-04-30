// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_time.h
 * \author  Olgierd Humenczuk
 * \brief   Time functions equavilents 
 */

#ifndef __XI_TIME_H__
#define __XI_TIME_H__

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

// used by the xively structures and local xi_ time.h equavilants
typedef long xi_time_t;

// used by the xi_mktime and xi_gmtime
struct xi_tm {
    int tm_sec;     /* seconds after the minute [0-61] */
    int tm_min;     /* minutes after the hour [0-59] */
    int tm_hour;    /* hours since midnight [0-23] */
    int tm_mday;    /* day of the month [1-31] */
    int tm_mon;     /* months since January [0-11] */
    int tm_year;    /* years since 1900 */
    int tm_wday;    /* days since Sunday [0-6] */
    int tm_yday;    /* days since January 1 [0-365] */
    int tm_isdst;   /* Daylight Savings Time flag */
    long tm_gmtoff; /* offset from UTC in seconds */
    char *tm_zone;  /* timezone abbreviation */
};

/**
 * \brief   Converts from `tm` to `xi_time_t`
 *
 * \note    This code assumes that unsigned long can be converted to `xi_time_t`.
 *          A `xi_time_t` should not be wider than `unsigned long`, since this
 *          would mean that the check for overflow at the end could fail.
 *
 * \note    This implementation had been copied from MINIX C library.
 *          Which is 100% compatible with our license.
 *
 * \note    This function does not take into account the timezone or the `dst`,
 *          it just converts `tm` structure using date and time fields (i.e. UTC).
 */
xi_time_t xi_mktime( struct xi_tm* t );

/**
 * \brief   This just wraps system's `gmtime()`, it a facade for future use
 */
struct xi_tm* xi_gmtime( register const xi_time_t* t );

#ifdef __cplusplus
}
#endif

#endif // __XI_TIME_H__
