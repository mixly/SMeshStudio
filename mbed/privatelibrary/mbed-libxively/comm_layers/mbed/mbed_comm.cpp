// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    mbed_comm.cpp
 * \author  Olgierd Humenczuk
 * \brief   Implements mbed _communication layer_ abstraction interface using [TCPSocketConnection](http://mbed.org/users/mbed_official/code/Socket/docs/tip/classTCPSocketConnection.html) [see comm_layer.h]
 */
 
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mbed_comm.h"
#include "comm_layer.h"
#include "xi_helpers.h"
#include "xi_allocator.h"
#include "mbed_comm_layer_data_specific.h"
#include "xi_debug.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_globals.h"

extern "C" {

connection_t* mbed_open_connection( const char* address, int32_t port )
{
    // PRECONDITIONS
    assert( address != 0 );

    // variables
    mbed_comm_layer_data_specific_t* pos_comm_data = 0;
    connection_t* conn                             = 0;

    // allocation of the socket connector
    TCPSocketConnection* socket_ptr                = new TCPSocketConnection();
    XI_CHECK_MEMORY( socket_ptr );

    // set the timeout for blocking operations
    socket_ptr->set_blocking( true, xi_globals.network_timeout );

    // allocate memory for the mbed data specific structure
    pos_comm_data = ( mbed_comm_layer_data_specific_t* )
            xi_alloc( sizeof( mbed_comm_layer_data_specific_t ) );
    XI_CHECK_MEMORY( pos_comm_data );

    // allocate memory for the connection layer
    conn = ( connection_t* ) xi_alloc(
                sizeof( connection_t ) );
    XI_CHECK_MEMORY( conn );

    // clean the memory before the usage
    memset( conn, 0, sizeof( connection_t ) );

    // make copy of an address
    conn->address = xi_str_dup( address );
    conn->port = port;

    XI_CHECK_MEMORY( conn->address );

    { // to prevent the skip initializtion warning
        pos_comm_data->socket_ptr = socket_ptr;

        // assign the layer specific data
        conn->layer_specific = pos_comm_data;

        // try to connect
        int s = pos_comm_data->socket_ptr->connect( address, port );

        // check if not failed
        if( s == -1 )
        {
            xi_set_err( XI_SOCKET_CONNECTION_ERROR );
            goto err_handling;
        }
    }

    // POSTCONDITIONS
    assert( conn != 0 );
    assert( pos_comm_data->socket_ptr != 0 );

    return conn;

    // cleanup the memory
err_handling:
    // safely destroy the object
    if ( pos_comm_data && pos_comm_data->socket_ptr )
    {
        delete pos_comm_data->socket_ptr;
        pos_comm_data->socket_ptr = 0;
    }
    if( pos_comm_data ) { XI_SAFE_FREE( pos_comm_data ); }
    if( conn ) { XI_SAFE_FREE( conn->address ); }
    XI_SAFE_FREE( conn );

    return 0;
}

int mbed_send_data( connection_t* conn, const char* data, size_t size )
{
    // PRECONDITIONS
    assert( conn != 0 );
    assert( conn->layer_specific != 0 );
    assert( data != 0 );
    assert( size != 0 );

    // extract the layer specific data
    mbed_comm_layer_data_specific_t* pos_comm_data
        = ( mbed_comm_layer_data_specific_t* ) conn->layer_specific;

    // Why not const char* ???
    int bytes_written = pos_comm_data->socket_ptr->send_all( ( char* ) data, size );

    if( bytes_written == - 1 )
    {
        xi_set_err( XI_SOCKET_WRITE_ERROR );
    }

    // store the value
    conn->bytes_sent += bytes_written;

    return bytes_written;
}

int mbed_read_data( connection_t* conn, char* buffer, size_t buffer_size )
{
    // PRECONDITIONS
    assert( conn != 0 );
    assert( conn->layer_specific != 0 );
    assert( buffer != 0 );
    assert( buffer_size != 0 );

    // extract the layer specific data
    mbed_comm_layer_data_specific_t* pos_comm_data
        = ( mbed_comm_layer_data_specific_t* ) conn->layer_specific;

    memset( buffer, 0, buffer_size );
    pos_comm_data->socket_ptr->set_blocking(true, 10);
    int bytes_read = pos_comm_data->socket_ptr->receive( buffer, buffer_size );

    if( bytes_read == -1 )
    {
        xi_set_err( XI_SOCKET_READ_ERROR );
    }

    // store the value
    conn->bytes_received += bytes_read;

    return bytes_read;
}

void mbed_close_connection( connection_t* conn )
{
    // PRECONDITIONS
    assert( conn != 0 );
    assert( conn->layer_specific != 0 );

    // extract the layer specific data
    mbed_comm_layer_data_specific_t* pos_comm_data
        = ( mbed_comm_layer_data_specific_t* ) conn->layer_specific;

    // close the connection & the socket
    if( pos_comm_data->socket_ptr->close() == -1 )
    {
        xi_set_err( XI_SOCKET_CLOSE_ERROR );
        goto err_handling;
    }

    // cleanup the memory
err_handling:
    // safely destroy the object
    if ( pos_comm_data && pos_comm_data->socket_ptr )
    {
        delete pos_comm_data->socket_ptr;
        pos_comm_data->socket_ptr = 0;
    }
    if( conn ) { XI_SAFE_FREE( conn->address ); }
    if( conn ) { XI_SAFE_FREE( conn->layer_specific ); }
    XI_SAFE_FREE( conn );
    return;
}

}
