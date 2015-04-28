// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_transport_layer.c
 * \author  Olgierd Humenczuk
 * \brief   Implements HTTP _transport layer_ encoders and decoders specific to Xively REST/HTTP API [see http_transport_layer.h]
 */

#include <string.h>
#include <stdio.h>

#include "http_transport.h"
#include "http_layer_queries.h"
#include "http_consts.h"
#include "http_layer_parser.h"
#include "xi_macros.h"
#include "xi_helpers.h"
#include "xi_debug.h"
#include "xi_err.h"

static char XI_HTTP_QUERY_BUFFER[ XI_QUERY_BUFFER_SIZE + XI_CONTENT_BUFFER_SIZE ];
static char XI_HTTP_QUERY_DATA[ XI_CONTENT_BUFFER_SIZE ];


inline static char* http_encode_concat( char* buffer, size_t buffer_size
    , const char* query, const char* content, const char* data )
{
    int offset  = 0;
    int size    = ( int ) buffer_size;
    int s       = 0;

    s = snprintf( buffer, size, "%s", query );
    XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_CREATE_DATASTREAM );

    if( content != 0 )
    {
        s = snprintf( buffer + offset
                          , XI_MAX( size - offset, 0 )
                          , "%s", content );
        XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_CREATE_DATASTREAM );
    }

    s = snprintf( buffer + offset
                      , XI_MAX( size - offset, 0 )
                      , "%s", XI_HTTP_CRLF );
    XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_CREATE_DATASTREAM );


    if( content != 0 && data != 0 )
    {
        s = snprintf( buffer + offset
                          , XI_MAX( size - offset, 0 )
                          , "%s", data );
        XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_CREATE_DATASTREAM );
    }

    s = snprintf( buffer + offset
                      , XI_MAX( size - offset, 0 )
                      , "%s", XI_HTTP_CRLF );

    XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_CREATE_DATASTREAM );

    return buffer;

err_handling:
    return 0;
}

const char* http_encode_create_datastream(
          const data_layer_t* data_transport
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* datapoint )
{
    // prepare parts
    const char* data = data_transport->encode_create_datastream(
        datastream_id, datapoint );

    if( data == 0 ) { return 0; }

    const char* query = http_construct_request_datastream(
              XI_HTTP_QUERY_POST, &feed_id
            , 0, x_api_key
    );

    if( query == 0 ) { return 0; }

    const char* content = http_construct_content( strlen( data ) );

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, content, data );
}

const char* http_encode_update_datastream(
          const data_layer_t* data_layer
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* datapoint )
{
    // prepare parts
    const char* data = data_layer->encode_datapoint( datapoint );

    if( data == 0 ) { return 0; }

    const char* query = http_construct_request_datastream(
              XI_HTTP_QUERY_PUT
            , &feed_id
            , datastream_id
            , x_api_key
    );

    if( query == 0 ) { return 0; }

    const char* content = http_construct_content( strlen( data ) );

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, content, data );
}

const char* http_encode_get_datastream(
          const data_layer_t* data_layer
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id )
{
    XI_UNUSED( data_layer );

    // prepare parts
    const char* query = http_construct_request_datastream(
              XI_HTTP_QUERY_GET
            , &feed_id
            , datastream_id
            , x_api_key );

    if( query == 0 ) { return 0; }

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, 0, 0 );
}

const char* http_encode_delete_datastream(
          const data_layer_t* data_layer
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id )
{
    XI_UNUSED( data_layer );

    // prepare parts
    const char* query = http_construct_request_datastream(
              XI_HTTP_QUERY_DELETE
            , &feed_id
            , datastream_id
            , x_api_key );

    if( query == 0 ) { return 0; }

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, 0, 0 );
}

const char* http_encode_delete_datapoint(
          const data_layer_t* data_transport
        , const char* x_api_key
        , xi_feed_id_t feed_id
        , const char *datastream_id
        , const xi_datapoint_t* o )
{
    XI_UNUSED( data_transport );

    struct xi_tm* ptm = xi_gmtime(
        ( xi_time_t* ) &o->timestamp.timestamp );

    int s = snprintf( XI_HTTP_QUERY_BUFFER
        , sizeof( XI_HTTP_QUERY_BUFFER )
        , "%s/datapoints/%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ"
        , datastream_id
        , ptm->tm_year + 1900
        , ptm->tm_mon + 1
        , ptm->tm_mday
        , ptm->tm_hour
        , ptm->tm_min
        , ptm->tm_sec
        , o->timestamp.micro );

    XI_CHECK_SIZE( s, ( int ) sizeof( XI_HTTP_QUERY_BUFFER )
        , XI_HTTP_ENCODE_DELETE_DATAPOINT );

    {
        // prepare parts
        const char* query = http_construct_request_datastream(
                  XI_HTTP_QUERY_DELETE
                , &feed_id
                , XI_HTTP_QUERY_BUFFER
                , x_api_key );

        if( query == 0 ) { return 0; }

        return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
            , query, 0, 0 );
    }

err_handling:
    return 0;
}

const char* http_encode_update_feed(
          const data_layer_t* data_layer
        , const char* x_api_key
        , const xi_feed_t* feed )
{
    // prepare buffer
    XI_CLEAR_STATIC_BUFFER( XI_HTTP_QUERY_DATA );

    // PRECONDITIONS
    assert( data_layer != 0 );
    assert( x_api_key != 0 );
    assert( feed != 0 );

    // variables initialization
    const char* content = 0;
    const char* query = 0;

    { // data part preparation
        int offset  = 0;
        int size    = sizeof( XI_HTTP_QUERY_DATA );
        int s       = 0;

        // for each datastream
        //      generate the list of datapoints that you want to update
        for( size_t i = 0; i < feed->datastream_count; ++i )
        {
            const xi_datastream_t* curr_datastream = &feed->datastreams[ i ];

            // for each datapoint
            for( size_t j = 0; j < curr_datastream->datapoint_count; ++j )
            {
                const xi_datapoint_t* curr_datapoint
                    = &curr_datastream->datapoints[ j ];

                // add the datastream id to the buffer
                s = snprintf(
                      XI_HTTP_QUERY_DATA + offset, XI_MAX( size - offset, 0 ), "%s,"
                    , curr_datastream->datastream_id );

                XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_UPDATE_FEED );

                // add the datapoint data to the buffer
                s = data_layer->encode_datapoint_in_place(
                      XI_HTTP_QUERY_DATA + offset, XI_MAX( size - offset, 0 )
                    , curr_datapoint );

                XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_UPDATE_FEED );
            }
        }
    }

    query = http_construct_request_feed(
          XI_HTTP_QUERY_PUT
        , &feed->feed_id
        , x_api_key
        , 0
    );

    if( query == 0 ) { goto err_handling; }

    content = http_construct_content( strlen( XI_HTTP_QUERY_DATA ) );

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, content, XI_HTTP_QUERY_DATA );

err_handling:
    return 0;
}

const char* http_encode_get_feed(
        const data_layer_t* data_layer
      , const char* x_api_key
      , const xi_feed_t* feed )
{
    // prepare buffer
    XI_CLEAR_STATIC_BUFFER( XI_HTTP_QUERY_DATA );

    // PRECONDITIONS
    assert( data_layer != 0 );
    assert( x_api_key != 0 );
    assert( feed != 0 );

    // variables initialization
    const char* query = 0;

    { // data part preparation
        int offset  = 0;
        int size    = sizeof( XI_HTTP_QUERY_DATA );
        int s       = 0;

        // for each datastream
        //      generate the list of datastreams that you want to get
        for( size_t i = 0; i < feed->datastream_count; ++i )
        {
            const xi_datastream_t* curr_datastream = &feed->datastreams[ i ];

            // add the datastream id to the buffer
            s = snprintf(
                  XI_HTTP_QUERY_DATA + offset, XI_MAX( size - offset, 0 ), i == 0 ? "?datastreams=%s" : ",%s"
                , curr_datastream->datastream_id );

            XI_CHECK_S( s, size, offset, XI_HTTP_ENCODE_UPDATE_FEED );
        }
    }

    query = http_construct_request_feed(
          XI_HTTP_QUERY_GET
        , &feed->feed_id
        , x_api_key
        , XI_HTTP_QUERY_DATA
    );

    if( query == 0 ) { goto err_handling; }

    return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
        , query, 0, 0 );

err_handling:
    return 0;
}

const char* http_encode_datapoint_delete_range(
        const data_layer_t* data_layer
      , const char* x_api_key
      , xi_feed_id_t feed_id
      , const char* datastream_id
      , const xi_timestamp_t* start
      , const xi_timestamp_t* end )
{
    XI_UNUSED( data_layer );

    struct xi_tm stm;
    struct xi_tm etm;

    {
        struct xi_tm* tmp = start ? xi_gmtime(
            ( xi_time_t* ) &start->timestamp ) : 0;
        memcpy( &stm, tmp, sizeof( struct xi_tm ) );
    }

    {
        struct xi_tm* tmp = end ? xi_gmtime(
            ( xi_time_t* ) &end->timestamp ) : 0;
        memcpy( &etm, tmp, sizeof( struct xi_tm ) );
    }

    int s = 0;

    if( start && end )
    {
        s = snprintf( XI_HTTP_QUERY_BUFFER
            , sizeof( XI_HTTP_QUERY_BUFFER )
            , "%s/datapoints?start=%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ&end=%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ"
            , datastream_id
            , stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday
            , stm.tm_hour, stm.tm_min, stm.tm_sec, start->micro
            , etm.tm_year + 1900, etm.tm_mon + 1, etm.tm_mday
            , etm.tm_hour, etm.tm_min, etm.tm_sec, end->micro );
    }
    else if( start )
    {
        s = snprintf( XI_HTTP_QUERY_BUFFER
            , sizeof( XI_HTTP_QUERY_BUFFER )
            , "%s/datapoints?start=%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ"
            , datastream_id
            , stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday
            , stm.tm_hour, stm.tm_min, stm.tm_sec, start->micro );
    }
    else if( end )
    {
        s = snprintf( XI_HTTP_QUERY_BUFFER
            , sizeof( XI_HTTP_QUERY_BUFFER )
            , "%s/datapoints?end=%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ"
            , datastream_id
            , etm.tm_year + 1900, etm.tm_mon + 1, etm.tm_mday
            , etm.tm_hour, etm.tm_min, etm.tm_sec, end->micro );
    }
    else
    {
        // just set an error
        XI_CHECK_CND( 1 == 0, XI_HTTP_ENCODE_DELETE_RANGE_DATAPOINT );
    }

    XI_CHECK_SIZE( s, ( int ) sizeof( XI_HTTP_QUERY_BUFFER )
        , XI_HTTP_ENCODE_DELETE_RANGE_DATAPOINT );

    {
        // prepare parts
        const char* query = http_construct_request_datastream(
                  XI_HTTP_QUERY_DELETE
                , &feed_id
                , XI_HTTP_QUERY_BUFFER
                , x_api_key );

        if( query == 0 ) { return 0; }

        return http_encode_concat( XI_HTTP_QUERY_BUFFER, sizeof( XI_HTTP_QUERY_BUFFER )
            , query, 0, 0 );
    }

err_handling:
    return 0;
}


const xi_response_t* http_decode_reply(
          const data_layer_t* data_layer
        , const char* response )
{
    XI_UNUSED( data_layer );

    static xi_response_t  __tmp;
    static http_response_t* __response = &__tmp.http;

    // just pass it further
    if( parse_http( __response, response ) == 0 )
    {
        return 0;
    }

    // pass it to the data_layer
    return &__tmp;
}
