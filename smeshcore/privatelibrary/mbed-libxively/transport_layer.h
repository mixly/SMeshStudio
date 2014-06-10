// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    transport_layer.h
 * \author  Olgierd Humenczuk
 * \brief   Defines _transport layer_ abstraction interface
 */

#ifndef __TRANSPORT_LAYER_H__
#define __TRANSPORT_LAYER_H__

#include "xively.h"
#include "data_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   _The transport layer interface_ - contains function pointers,
 *          that's what we expose to the layers above and below
 *
 *    It is effectively a class that holds declarations of pure virtual functions.
 *    * All functions take `data_layer_t` as the first argument.
 *    * Most encoders convert the result from data layer to an implementation-specific representaion.
 *
 * \note    It depends on the implementation whether any given _transport layer_ method will call upon
 *          the _data layer_. In `http_transport_layer.c` you can see that `http_encode_datapoint_delete_range()`
 *          only needs to generate request headers and no body needs to be posted, neither it gets any
 *          response body as it is doing a `DELETE` request. One should use `XI_UNUSED(data_layer)` at the
 *          top of the function definition, that is a macro that casts the pointer to unused _data layer_ to void.
 * \note    Similarly to the _data layer_ (see notes in `data_layer.h`), there no symmetry needed and we only have
 *          one decoder.
 */
typedef struct {
    const char* ( *encode_update_feed )(
          const data_layer_t*, const char* api_key
        , const xi_feed_t* feed );

    const char* ( *encode_get_feed )(
          const data_layer_t*, const char* api_key
        , const xi_feed_t* feed );

    const char* ( *encode_create_datastream )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id
        , const xi_datapoint_t* dp );

    const char* ( *encode_update_datastream )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id
        , const xi_datapoint_t* value );

    const char* ( *encode_get_datastream )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id );

    const char* ( *encode_delete_datastream )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id );

    const char* ( *encode_delete_datapoint )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id
        , const xi_datapoint_t* datapoint );

    const char* ( *encode_datapoint_delete_range )(
          const data_layer_t*, const char* api_key, xi_feed_id_t feed_id
        , const char* datastream_id
        , const xi_timestamp_t* start
        , const xi_timestamp_t* end );

    const xi_response_t* ( *decode_reply )(
        const data_layer_t*, const char* data );
} transport_layer_t;

#ifdef __cplusplus
}
#endif

#endif // __TRANSPORT_LAYER_H__
