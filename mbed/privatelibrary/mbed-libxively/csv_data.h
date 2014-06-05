// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    csv_data.h
 * \author  Olgierd Humenczuk
 * \brief   Implements CSV _data layer_ encoders and decoders specific to Xively CSV data format
 */
 
#ifndef __CSV_DATA_H__
#define __CSV_DATA_H__

#include "xively.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* csv_encode_datapoint( const xi_datapoint_t* dp );


int csv_encode_datapoint_in_place(
      char* buffer, size_t buffer_size
    , const xi_datapoint_t* datapoint );

const char* csv_encode_create_datastream(
          const char* buffer
        , const xi_datapoint_t* dp );

xi_feed_t* csv_decode_feed(
      const char* buffer
    , xi_feed_t* feed );

xi_datapoint_t* csv_decode_datapoint( const char* data, xi_datapoint_t* dp );

#ifdef __cplusplus
}
#endif

#endif // __CSV_DATA_H__
