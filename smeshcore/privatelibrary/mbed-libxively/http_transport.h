// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_transport.h
 * \author  Olgierd Humenczuk
 * \brief   Implements HTTP _transport layer_ abstraction interface
 */

#ifndef __HTTP_TRANSPORT_H__
#define __HTTP_TRANSPORT_H__

#include "transport_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * \brief   Initialise HTTP implementation of the _transport layer_
 *
 *    This intialiser assigns function pointers to the actual implementations
 *    using  static function variable trick, hence the intialisation should
 *    not give any overhead.
 *
 * \return  Structure with function pointers for HTTP encoders and decoders
 *          which had been implemented in `http_transport_layer.c`.
 */
transport_layer_t* get_http_transport_layer( void );

#ifdef __cplusplus
}
#endif

#endif // __HTTP_TRANSPORT_H__
