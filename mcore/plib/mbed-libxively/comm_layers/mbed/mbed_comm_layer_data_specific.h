// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    mbed_comm_layer_data_specific.h
 * \author  Olgierd Humenczuk
 * \brief   Declares layer-specific data structure
 */

#ifndef __MBED_COMM_LAYER_DATA_SPECIFIC_H__
#define __MBED_COMM_LAYER_DATA_SPECIFIC_H__

#include "TCPSocketConnection.h"

typedef struct {
    TCPSocketConnection* socket_ptr;
} mbed_comm_layer_data_specific_t;

#endif // __MBED_COMM_LAYER_DATA_SPECIFIC_H__
