// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    csv_data_layer.h
 * \author  Olgierd Humenczuk
 * \brief   Implements CSV _data layer_ abstration interface
 */

#ifndef __CSV_DATA_LAYER_H__
#define __CSV_DATA_LAYER_H__

#include "xively.h"
#include "data_layer.h"
#include "csv_data.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * \brief   Initialise CSV implementation of the _data layer_
 *
 *    This intialiser assigns function pointers to the actual implementations
 *    using  static function variable trick, hence the intialisation should
 *    not give any overhead.
 *
 * \return  Structure with function pointers for CSV encoders and decoders
 *          which had been implemented in `csv_data.c`.
 */
const data_layer_t* get_csv_data_layer( void );

#ifdef __cplusplus
}
#endif

#endif // __CSV_DATA_LAYER_H__
