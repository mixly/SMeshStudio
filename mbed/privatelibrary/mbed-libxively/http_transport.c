// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_transport.c
 * \author  Olgierd Humenczuk
 * \brief   Implements HTTP _transport layer_ abstraction interface [see http_transport.h and transport_layer.h]
 */

#include "http_transport_layer.h"
#include "http_transport.h"

transport_layer_t* get_http_transport_layer( void )
{
    static transport_layer_t __http_transport_layer =
    {
          &http_encode_update_feed
        , &http_encode_get_feed
        , &http_encode_create_datastream
        , &http_encode_update_datastream
        , &http_encode_get_datastream
        , &http_encode_delete_datastream
        , &http_encode_delete_datapoint
        , &http_encode_datapoint_delete_range
        , &http_decode_reply
    };

    return &__http_transport_layer;
}
