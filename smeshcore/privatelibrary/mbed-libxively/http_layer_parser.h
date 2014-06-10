// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_layer_parser.h
 * \author  Olgierd Humenczuk
 * \brief   Our simple HTTP parser
 */

#ifndef __HTTP_LAYER_PARSER_H__
#define __HTTP_LAYER_PARSER_H__

#include "xi_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  This function takes the pointer to the `http_response_t` structure and
 *         fills it with parsed data from the give buffer.
 *
 *    While the parser looks at headers, satus line and content, it populates given
 *    pointer to `http_response_t`.
 *
 * \return Pointer or null if an error occurred.
 *
 * \note   It currently won't work against partial data.
 */
http_response_t* parse_http( http_response_t* response, const char* data );

#ifdef __cplusplus
}
#endif

#endif
