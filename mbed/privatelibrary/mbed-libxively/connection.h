// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    connection.h
 * \author  Olgierd Humenczuk
 * \brief   Defines `connection_t`, which is used by the _communication layer_
 *
 *    It is designed to abstract from implementation- and/or platform- specific
 *    ways of storing connection info, such as host and port, as well as simple
 *    statistics counters. It also provides arbitary pointer to implementation's
 *    own strucutre.
 */

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   _The connection structure_ - holds data needed for further processing
 *          and error handling
 * 
 * It also contain `layer_specific` field which should point at the platform's
 * structure, according to the implementation of that specific _communication_
 * _layer_ and other implementation don't need to care about what that is.
 *
 * The purpose of that class is to give the abstract interface of a connection
 * that can be easly used with the `comm_layer_t` interface, so that it's possible
 * to send/receive data to/from the server through different communication layer
 * using the same interface.
 */
typedef struct {
    void    *layer_specific; //!< here the layer can hide some layer specific data
    char    *address;        //!< here we store server's address
    int      port;           //!< here we store server's port
    size_t   bytes_sent;     //!< the data sent counter, just for testing and statistics
    size_t   bytes_received; //!< the data receive counter, just for tests and statistics
} connection_t;

#ifdef __cplusplus
}
#endif

#endif // __CONNECTION_H__
