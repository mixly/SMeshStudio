// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file 	mbed_comm_layer.c
 * \author 	Olgierd Humenczuk
 * \brief   Implements mbed _communication layer_ abstraction interface [see comm_layer.h]
 */

#include "comm_layer.h"
#include "mbed_comm.h"

 /**
  * \brief   Initialise POSIX implementation of the _communication layer_
  */
const comm_layer_t* get_comm_layer()
{
    static comm_layer_t __mbed_comm_layer =
    {
          &mbed_open_connection
        , &mbed_send_data
        , &mbed_read_data
        , &mbed_close_connection
    };

    return &__mbed_comm_layer;
}
