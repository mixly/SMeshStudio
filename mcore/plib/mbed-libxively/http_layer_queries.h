// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_layer_queries.c
 * \author  Olgierd Humenczuk
 * \brief   Helpers for making HTTP requests (specific to Xively REST/HTTP API)
 *
 *    * All functions return pointer to the buffer with request string or null in case of any error.

 * \warning The buffer is managed by the library, so it's forbidden to free the pointer.
 */

#ifndef __HTTP_LAYERS_QUERIES_H__
#define __HTTP_LAYERS_QUERIES_H__

#include <stdlib.h>
#include <stdint.h>

#include <xively.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* http_construct_request_datapoint(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* datastream_id
        , const char* dp_ts_str
        , const char* x_api_key );

const char* http_construct_request_datastream(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* datastream_id
        , const char* x_api_key );

const char* http_construct_request_feed(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* x_api_key
        , const char* query_suffix );

const char* http_construct_content(
          int32_t content_size );

#ifdef __cplusplus
}
#endif

#endif // __HTTP_LAYERS_QUERIES_H__
