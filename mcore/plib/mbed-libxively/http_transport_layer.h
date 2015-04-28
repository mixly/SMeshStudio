// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_transport_layer.h
 * \author  Olgierd Humenczuk
 * \brief   Implements HTTP _transport layer_ encoders and decoders specific to Xively REST/HTTP API
 */

#ifndef __HTTP_TRANSPORT_LAYER_H__
#define __HTTP_TRANSPORT_LAYER_H__

#include "xively.h"
#include "data_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* http_encode_create_datastream(
          const data_layer_t*
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* value );

const char* http_encode_update_datastream(
          const data_layer_t*
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* value );

const char* http_encode_get_datastream(
          const data_layer_t*
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id );

const char* http_encode_delete_datastream(
          const data_layer_t*
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id );

const char* http_encode_delete_datapoint(
          const data_layer_t*
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* o );

const char* http_encode_update_feed(
          const data_layer_t*
        , const char* x_api_key
        , const xi_feed_t* feed );

const char* http_encode_get_feed(
        const data_layer_t*
      , const char* x_api_key
      , const xi_feed_t* feed );

const char* http_encode_datapoint_delete_range(
        const data_layer_t*
      , const char* x_api_key
      , xi_feed_id_t feed_id
      , const char* datastream_id
      , const xi_timestamp_t* start
      , const xi_timestamp_t* end );

const xi_response_t* http_decode_reply(
          const data_layer_t*
        , const char* data );

#ifdef __cplusplus
}
#endif

#endif // __HTTP_TRANSPORT_LAYER_H__
