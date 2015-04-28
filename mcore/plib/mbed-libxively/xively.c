// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xively.c
 * \brief   Xively C library [see xively.h]
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xi_allocator.h"
#include "xively.h"
#include "http_transport.h"
#include "csv_data_layer.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_helpers.h"
#include "xi_err.h"
#include "xi_globals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Get instance of _communication layer_
 *
 * \note    Although the interface is of the _communication layer_ should
 *          stay the same, some implemantation may differ.
 *
 * \return  Pointer to the communication layer interface
 */
const comm_layer_t* get_comm_layer( void );

#include "comm_layer.h"

//-----------------------------------------------------------------------
// HELPER MACROS
//-----------------------------------------------------------------------

#define XI_FUNCTION_VARIABLES connection_t* conn = 0;\
    const comm_layer_t* comm_layer = 0;\
    const transport_layer_t* transport_layer = 0;\
    const data_layer_t* data_layer = 0;\
    char  buffer[ XI_HTTP_MAX_CONTENT_SIZE ];\
    const xi_response_t* response = 0;\
    int sent = 0;\
    int recv = 0;

#define XI_FUNCTION_PROLOGUE  XI_FUNCTION_VARIABLES\
    xi_debug_logger( "Getting the comm layer..." );\
    comm_layer = get_comm_layer();\
    xi_debug_logger( "Getting the transport layer..." );\
    transport_layer = get_http_transport_layer();\
    xi_debug_logger( "Getting the data layer...");\
    data_layer = get_csv_data_layer();\

#define XI_FUNCTION_GET_RESPONSE if( data == 0 ) { goto err_handling; }\
    xi_debug_logger( "Connecting to the endpoint..." );\
    conn = comm_layer->open_connection( XI_HOST, XI_PORT );\
    if( conn == 0 ) { goto err_handling; }\
    xi_debug_logger( "Sending data:" );\
    xi_debug_printf( "%s\r\n", data );\
    sent = comm_layer->send_data( conn, data, strlen( data ) );\
    if( sent == -1 ) { goto err_handling; }\
    xi_debug_printf( "Sent: %d\r\n", ( int ) sent );\
    xi_debug_logger( "Reading data..." );\
    recv = comm_layer->read_data( conn, buffer, XI_HTTP_MAX_CONTENT_SIZE );\
    if( recv == -1 ) { goto err_handling; }\
    xi_debug_printf( "Received: %d\r\n", ( int ) recv );\
    xi_debug_logger( "Response:" );\
    xi_debug_printf( "%s\r\n", buffer );\
    response = transport_layer->decode_reply(\
        data_layer, buffer );\
    if( response == 0 ) { goto err_handling; }\

#define XI_FUNCTION_EPILOGUE xi_debug_logger( "Closing connection..." );\
err_handling:\
    if( conn )\
    {\
        comm_layer->close_connection( conn );\
    }\
    return response;\

//-----------------------------------------------------------------------
// HELPER FUNCTIONS
//-----------------------------------------------------------------------

xi_value_type_t xi_get_value_type( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    return p->value_type;
}

xi_datapoint_t* xi_set_value_i32( xi_datapoint_t* p, int32_t value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.i32_value  = value;
    p->value_type       = XI_VALUE_TYPE_I32;

    return p;
}

int32_t xi_get_value_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_I32 );

    return p->value.i32_value;
}

int32_t* xi_value_pointer_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_I32 )
    {
      return &p->value.i32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_f32( xi_datapoint_t* p, float value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.f32_value  = value;
    p->value_type       = XI_VALUE_TYPE_F32;

    return p;
}

float xi_get_value_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_F32 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return p->value.f32_value;
    }
    else
    {
      return 0.;
    }
}

float* xi_value_pointer_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return &p->value.f32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_str( xi_datapoint_t* p, const char* value )
{
    // PRECONDITION
    assert( p != 0 );

    int s = xi_str_copy_untiln( p->value.str_value
        , XI_VALUE_STRING_MAX_SIZE, value, '\0' );

    XI_CHECK_SIZE( s, XI_VALUE_STRING_MAX_SIZE, XI_DATAPOINT_VALUE_BUFFER_OVERFLOW );

    p->value_type = XI_VALUE_TYPE_STR;

    return p;

err_handling:
    return 0;
}

char* xi_value_pointer_str( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_STR )
    {
      return p->value.str_value;
    }
    else
    {
      return NULL;
    }
}

void xi_set_network_timeout( uint32_t timeout )
{
    xi_globals.network_timeout = timeout;
}

uint32_t xi_get_network_timeout( void )
{
    return xi_globals.network_timeout;
}

//-----------------------------------------------------------------------
// MAIN LIBRARY FUNCTIONS
//-----------------------------------------------------------------------

xi_context_t* xi_create_context(
      xi_protocol_t protocol, const char* api_key
    , xi_feed_id_t feed_id )
{
    // allocate the structure to store new context
    xi_context_t* ret = ( xi_context_t* ) xi_alloc( sizeof( xi_context_t ) );

    XI_CHECK_MEMORY( ret );

    // copy given numeric parameters as is
    ret->protocol       = protocol;
    ret->feed_id        = feed_id;

    // copy string parameters carefully
    if( api_key )
    {
        // duplicate the string
        ret->api_key  = xi_str_dup( api_key );

        XI_CHECK_MEMORY( ret->api_key );
    }
    else
    {
        ret->api_key  = 0;
    }

    return ret;

err_handling:
    if( ret )
    {
        XI_SAFE_FREE( ret );
    }

    return 0;
}

void xi_delete_context( xi_context_t* context )
{
    if( context )
    {
        XI_SAFE_FREE( context->api_key );
    }
    XI_SAFE_FREE( context );
}

const xi_response_t* xi_feed_get(
          xi_context_t* xi
        , xi_feed_t* feed )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_get_feed(
              data_layer
            , xi->api_key
            , feed );

    XI_FUNCTION_GET_RESPONSE

    feed = data_layer->decode_feed( response->http.http_content, feed );
    if( feed == 0 ) { goto err_handling; }

    XI_FUNCTION_EPILOGUE
}

const xi_response_t* xi_feed_update(
          xi_context_t* xi
        , const xi_feed_t* feed )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_update_feed(
              data_layer
            , xi->api_key
            , feed );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}

const xi_response_t* xi_datastream_get(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id, xi_datapoint_t* o )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_get_datastream(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id );

    XI_FUNCTION_GET_RESPONSE

    o = data_layer->decode_datapoint(
        response->http.http_content, o );

    if( o == 0 ) { goto err_handling; }

    XI_FUNCTION_EPILOGUE
}


const xi_response_t* xi_datastream_create(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_datapoint_t* datapoint )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_create_datastream(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id
            , datapoint );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}

const xi_response_t* xi_datastream_update(
          xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* datapoint )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_update_datastream(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id
            , datapoint );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}
const xi_response_t* xi_datastream_delete(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_delete_datastream(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}

const xi_response_t* xi_datapoint_delete(
          const xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* o )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_delete_datapoint(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id
            , o );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}

extern const xi_response_t* xi_datapoint_delete_range(
            const xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_timestamp_t* start
          , const xi_timestamp_t* end )
{
    XI_FUNCTION_PROLOGUE

    const char* data = transport_layer->encode_datapoint_delete_range(
              data_layer
            , xi->api_key
            , feed_id
            , datastream_id
            , start
            , end );

    XI_FUNCTION_GET_RESPONSE

    XI_FUNCTION_EPILOGUE
}


#ifdef __cplusplus
}
#endif
