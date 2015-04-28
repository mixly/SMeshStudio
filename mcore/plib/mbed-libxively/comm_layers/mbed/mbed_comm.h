// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file 	mbed_comm.h
 * \author 	Olgierd Humenczuk
 * \brief   Implements mbed _communication layer_ functions [see comm_layer.h and mbed_comm.cpp]
 */

#ifndef __MBED_COMM_H__
#define __MBED_COMM_H__

#include "connection.h"

#ifdef __cplusplus
extern "C" {
#endif

connection_t* mbed_open_connection( const char* address, int32_t port );

int mbed_send_data( connection_t* conn, const char* data, size_t size );

int mbed_read_data( connection_t* conn, char* buffer, size_t buffer_size );

void mbed_close_connection( connection_t* conn );

#ifdef __cplusplus
}
#endif

#endif // __MBED_COMM_H__
