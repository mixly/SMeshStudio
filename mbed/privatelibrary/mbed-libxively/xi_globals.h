// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_globals.h
 * \author  Olgierd Humenczuk
 * \brief   Global run-time settings used by the library
 */

#ifndef __XI_GLOBALS_H__
#define __XI_GLOBALS_H__


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Global run-time settings structure
 */
typedef struct
{
    uint32_t network_timeout; //!< the network timeout (default: 1500 milliseconds)
} xi_globals_t;

extern xi_globals_t xi_globals; //!< global instance of `xi_globals_t`

#ifdef __cplusplus
}
#endif

#endif // __XI_GLOBALS_H__