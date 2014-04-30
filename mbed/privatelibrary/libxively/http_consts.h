// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_consts.h
 * \author  Olgierd Humenczuk
 * \brief   Constants used by our HTTP parser.
 */

#ifndef __HTTP_CONSTS_H__
#define __HTTP_CONSTS_H__

static const char XI_HTTP_CRLF[]           = "\r\n";
static const char XI_HTTP_CRLFX2[]         = "\r\n\r\n";
static const char XI_HTTP_QUERY_GET[]      = "GET";
static const char XI_HTTP_QUERY_PUT[]      = "PUT";
static const char XI_HTTP_QUERY_POST[]     = "POST";
static const char XI_HTTP_QUERY_DELETE[]   = "DELETE";

#endif // __HTTP_CONSTS_H__
