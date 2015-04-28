// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file	comm_layer.h
 * \author  Olgierd Humenczuk
 * \brief   Defines _communication layer_ abstraction interface
 *
 *    The design of _communication layer_ was based on Berkley/POSIX socket API.
 */

#ifndef __POSIX_COMM_LAYER_H__
#define __POSIX_COMM_LAYER_H__


#include <stdlib.h>
#include <stdint.h>

#include "connection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   _The communication layer interface_ - contains function pointers,
 *          that's what we expose to the layers above and below
 *
 *    It is effectively a class that holds declarations of pure virtual functions.
 *
 *   The interface contain methods related to connecting/disconnecting as well as
 *   reading/writing data to/from the remote endpoint.
 *
 *   This interface uses abstract `connection_t` class to hold information
 *   about the connection in a platform-independent fashion.
 */
typedef struct {
    /**
     * \brief   Connect to a given host using it's address and port
     * \note    It should not reset an existing connection.
     *
     * \return Pointer to `connection_t` or `0` in case of an error.
     */
    connection_t* ( *open_connection )( const char* address, int32_t port );

    /**
     * \brief   Send data over the connection
     *
     * \return  Number of bytes sent or `-1` in case of an error.
     */
    int ( *send_data )( connection_t* conn, const char* data, size_t size );

    /**
     * \brief   Read data from a connection
     *
     * \return  Number of bytes read or `-1` in case of an error.
     */
    int ( *read_data )( connection_t* conn, char* buffer, size_t buffer_size );

    /**
     * \brief   Close connection and free all allocated memory (if any)
     * \note    Some implementations may be stack-based as only limited
     *          number of connections is expected in a typical use-case.
     */
    void ( *close_connection )( connection_t* conn );
} comm_layer_t;


 /**
  * \brief   Initialise an implementation of the _communication layer_
  *
  *    This intialiser assigns function pointers to the actual implementations
  *    using  static function variable trick, hence the intialisation should
  *    not give any overhead.
  *
  * \return  Structure with function pointers for platform-specific communication
  *          methods (see `mbed_comm.h` and `posix_comm.h` for how it's done).
  */
const comm_layer_t* get_comm_layer( void );

#ifdef __cplusplus
}
#endif

#endif // __POSIX_COMM_LAYER_H__
