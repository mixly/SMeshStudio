// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_layer_queries.h
 * \author  Olgierd Humenczuk
 * \brief   Helpers for making HTTP requests (specific to Xively REST/HTTP API) [see http_layer_queries.h]
 */

#include <stdio.h>

#include "http_layer_queries.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_err.h"
#include "xi_config.h"
#include "xively.h"


static const char XI_HTTP_TEMPLATE_FEED[] = "%s /v2/feeds%s.csv%s HTTP/1.1\r\n"
                                  "Host: %s\r\n"
                                  "User-Agent: %s\r\n"
                                  "Accept: */*\r\n"
                                  "X-ApiKey: %s\r\n";

static const char XI_HTTP_ID_TEMPLATE[]    = "/%s";
static const char XI_HTTP_ID_TEMPLATE_D[]  = "/%d";

static const char XI_HTTP_CONTENT_TEMPLATE[] = "Content-Type: text/plain\r\n"
                                            "Content-Length: %d\r\n";

static char XI_QUERY_BUFFER[ XI_QUERY_BUFFER_SIZE ];
static char XI_CONTENT_BUFFER[ XI_CONTENT_BUFFER_SIZE ];
static char XI_ID_BUFFER[ XI_ID_BUFFER_SIZE ];

inline static int http_construct_string(
      char* buffer, size_t buffer_size
    , const char* string_id )
{
    // PRECONDITIONS
    assert( buffer      != 0 );
    assert( buffer_size != 0 );

    int s       = 0;
    int size    = buffer_size;

    if( string_id )
    {
        s = snprintf( buffer, size
            , XI_HTTP_ID_TEMPLATE, string_id );

        XI_CHECK_SIZE( s, size, XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );
    }

    return s;

err_handling:
    return -1;
}

inline static int http_construct_int(
      char* buffer, size_t buffer_size
    , const xi_feed_id_t* feed_id )
{
    // PRECONDITIONS
    assert( buffer      != 0 );
    assert( buffer_size != 0 );

    int s = 0;
    int size = buffer_size;

    if( feed_id )
    {
        s = snprintf( buffer, size
            , XI_HTTP_ID_TEMPLATE_D, *feed_id );

        XI_CHECK_SIZE( s, size, XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN )
    }

    return s;

err_handling:
    return -1;
}

inline static int http_construct_feed(
      char* buffer, size_t buffer_size
    , const xi_feed_id_t* feed_id )
{
    // PRECONDITIONS
    assert( buffer != 0 );
    assert( buffer_size != 0 );

    if( feed_id )
    {
        return http_construct_int( buffer
            , buffer_size, feed_id );
    }

    return 0;
}

inline static int http_construct_datastream(
      char* buffer, size_t buffer_size
    , const xi_feed_id_t* feed_id
    , const char* datastream_id )
{
    // PRECONDITIONS
    assert( buffer != 0 );
    assert( buffer_size != 0 );

    int s       = http_construct_feed( buffer, buffer_size, feed_id );
    int offset  = 0;
    int size    = buffer_size;

    XI_CHECK_S( s, size, offset, XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );

    s = http_construct_string( buffer + offset, buffer_size - offset
        , "datastreams" );

    XI_CHECK_S( s, size, offset, XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );

    if( datastream_id )
    {
        return http_construct_string( buffer + offset, buffer_size - offset
            , datastream_id == 0 ? "" : datastream_id );
    }

    return offset;

err_handling:
    return -1;
}

inline static int http_construct_datapoint(
      char* buffer, size_t buffer_size
    , const xi_feed_id_t* feed_id
    , const char* datastream_id
    , const char* datapoint )
{
    // PRECONDITIONS
    assert( buffer != 0 );
    assert( buffer_size != 0 );

    int s       = http_construct_datastream( buffer, buffer_size
        , feed_id, datastream_id );
    int offset  = 0;
    int size    = buffer_size;

    XI_CHECK_S( s, size, offset
        , XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );

    s = http_construct_string( buffer + offset, buffer_size - offset
        , "datapoints" );

    XI_CHECK_S( s, size, offset
        , XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );

    if( datapoint )
    {
        return http_construct_string( buffer + offset, buffer_size - offset
            , datapoint == 0 ? "" : datapoint );
    }

    return offset;

err_handling:
    return -1;
}

inline static const char* http_construct_http_query(
      const char* http_method
    , const char* id
    , const char* query_suffix
    , const char* x_api_key )
{
    // PRECONDITIONS
    assert( http_method     != 0 );
    assert( x_api_key   != 0 );

    int s = snprintf( XI_QUERY_BUFFER, XI_QUERY_BUFFER_SIZE, XI_HTTP_TEMPLATE_FEED
        , http_method, id == 0 ? "" : id, query_suffix == 0 ? "" : query_suffix
        , XI_HOST, XI_USER_AGENT, x_api_key );

    XI_CHECK_SIZE( s, XI_QUERY_BUFFER_SIZE
        , XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN );

    return XI_QUERY_BUFFER;

err_handling:
    return 0;
}

const char* http_construct_request_datapoint(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* datastream
        , const char* datapoint
        , const char* x_api_key )
{
    // PRECONDITIONS
    assert( http_method != 0 );
    assert( feed_id != 0 );
    assert( datastream != 0 );
    assert( x_api_key != 0 );

    int s = http_construct_datapoint( XI_ID_BUFFER, XI_ID_BUFFER_SIZE
        , feed_id, datastream, datapoint );

    XI_CHECK_SIZE( s, XI_ID_BUFFER_SIZE
        , XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN );

    return http_construct_http_query( http_method, XI_ID_BUFFER, 0, x_api_key );

err_handling:
    return 0;
}

const char* http_construct_request_datastream(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* datastream
        , const char* x_api_key )
{
    // PRECONDITIONS
    assert( http_method != 0 );
    assert( feed_id != 0 );
    assert( x_api_key != 0 );

    int s = http_construct_datastream( XI_ID_BUFFER, XI_ID_BUFFER_SIZE
        , feed_id, datastream );

    XI_CHECK_SIZE( s, XI_ID_BUFFER_SIZE
        , XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN )

    return http_construct_http_query( http_method, XI_ID_BUFFER, 0, x_api_key );

err_handling:
    return 0;
}

const char* http_construct_request_feed(
          const char* http_method
        , const xi_feed_id_t* feed_id
        , const char* x_api_key
        , const char* query_suffix )
{
    // PRECONDITIONS
    assert( http_method != 0 );
    assert( x_api_key != 0 );

    int s = http_construct_feed( XI_ID_BUFFER, XI_ID_BUFFER_SIZE, feed_id );

    XI_CHECK_SIZE( s, XI_ID_BUFFER_SIZE
        , XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN );

    return http_construct_http_query( http_method, XI_ID_BUFFER, query_suffix, x_api_key );

err_handling:
    return 0;
}

const char* http_construct_content(
          int32_t content_size )
{

    int s = snprintf( XI_CONTENT_BUFFER, XI_CONTENT_BUFFER_SIZE
      , XI_HTTP_CONTENT_TEMPLATE, content_size );

    XI_CHECK_SIZE( s, XI_CONTENT_BUFFER_SIZE
        , XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN );

    return XI_CONTENT_BUFFER;

err_handling:
    return 0;
}
