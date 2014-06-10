// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    http_layer_parser.c
 * \author  Olgierd Humenczuk
 * \brief   Our simple HTTP parser [see http_layer_parser.h]
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xively.h"
#include "xi_macros.h"
#include "http_consts.h"
#include "http_layer_parser.h"
#include "xi_debug.h"
#include "xi_err.h"

static const char XI_HTTP_STATUS_PATTERN[] =
    "HTTP/%d.%d %d %" XI_STR(XI_HTTP_STATUS_STRING_SIZE) "[^\r\n]\r\n"; //!< the match pattern

#define SET_HTTP_STATUS_PATTERN(a,b,c,d) XI_HTTP_STATUS_PATTERN, &a, &b, &c, d

static const char XI_HTTP_TOKEN_NAMES[ XI_HTTP_HEADERS_COUNT ][ XI_HTTP_HEADER_NAME_MAX_SIZE ] =
    {
          "date"            // XI_HTTP_HEADER_DATE
        , "content-type"    // XI_HTTP_HEADER_CONTENT_TYPE
        , "content-length"  // XI_HTTP_HEADER_CONTENT_LENGTH
        , "connection"      // XI_HTTP_HEADER_CONNECTION
        , "x-request-id"    // XI_HTTP_HEADER_X_REQUEST_ID
        , "cache-control"   // XI_HTTP_HEADER_CACHE_CONTROL
        , "age"             // XI_HTTP_HEADER_AGE
        , "vary"            // XI_HTTP_HEADER_VARY
        , "unknown"         // XI_HTTP_HEADER_UNKNOWN, //!< !!!! this must be always on the last position
    };

static inline http_header_type_t classify_header( const char* header )
{
    for( unsigned short i = 0; i < XI_HTTP_HEADER_COUNT - 1; ++i )
    {
        if( strcasecmp( header, XI_HTTP_TOKEN_NAMES[ i ] ) == 0 )
            return ( http_header_type_t ) i;
    }

    return XI_HTTP_HEADER_UNKNOWN;
}

const char* parse_http_status( http_response_t* response, const char* content )
{
    // variables
    int c = 0;

    // find the first occurrence of CRLF
    const char* header_end_ptr = strstr( content, XI_HTTP_CRLF );

    // check continuation condition
    XI_CHECK_ZERO( header_end_ptr, XI_HTTP_STATUS_PARSE_ERROR );

    // update the pointer
    header_end_ptr += 2;

    // parse
    c = sscanf( content
        , SET_HTTP_STATUS_PATTERN(
              response->http_version1
            , response->http_version2
            , response->http_status
            , response->http_status_string ) );

    // check if all arguments found
    XI_CHECK_CND( c != 4, XI_HTTP_STATUS_PARSE_ERROR );

    // return updated ptr
    return header_end_ptr;

err_handling:
    return 0;
}

const char* parse_http_header( http_response_t* response
    , const char* content )
{
    // find the first occurrence of CRLF
    const char* header_end_ptr      = strstr( content, XI_HTTP_CRLF );
    const char* header_name_end_ptr = strstr( content, ":" );

    // check continuation condition
    XI_CHECK_ZERO( header_end_ptr, XI_HTTP_HEADER_PARSE_ERROR );
    XI_CHECK_ZERO( header_name_end_ptr, XI_HTTP_HEADER_PARSE_ERROR );
    XI_CHECK_CND( header_name_end_ptr > header_end_ptr, XI_HTTP_HEADER_PARSE_ERROR );

    {
        int size = sizeof( response->http_headers[ response->http_headers_size ].name );

        XI_CHECK_CND( header_name_end_ptr - content > size - 1, XI_HTTP_HEADER_PARSE_ERROR );

        memcpy( response->http_headers[ response->http_headers_size ].name
            , content, XI_MIN( size - 1, header_name_end_ptr - content ) );

        // set the guard
        XI_GUARD_EOS( response->http_headers[ response->http_headers_size ].name, size );
    }

    // update the pointer
    header_name_end_ptr += 2;

    {
        int size = sizeof( response->http_headers[ response->http_headers_size ].value );

        XI_CHECK_CND( header_end_ptr - header_name_end_ptr > size - 1, XI_HTTP_HEADER_PARSE_ERROR );

        memcpy( response->http_headers[ response->http_headers_size ].value
            , header_name_end_ptr, XI_MIN( size - 1, header_end_ptr - header_name_end_ptr ) );

        // set the guard
        XI_GUARD_EOS( response->http_headers[ response->http_headers_size ].value, size );
    }

    // @TODO change the complexity of the classify header
    // implementation now it's O( n*m ) it can be done in O( n )
    // using some sort of simple DFA
    // I'm working on a very simple Python DFA generator for C
    // but still this is a w.i.p. at the moment

    // parse the header name
    {
        http_header_type_t ht = classify_header(
            response->http_headers[ response->http_headers_size ].name );

        // accept headers that differs from unknown
        if( ht != XI_HTTP_HEADER_UNKNOWN )
        {
            response->http_headers_checklist[ ht ] =
                &response->http_headers[ response->http_headers_size ];
        }

        // set header type
        response->http_headers[ response->http_headers_size ].header_type = ht;

        // increment headers size and check if it's okay
        response->http_headers_size += 1;

        XI_CHECK_CND( response->http_headers_size >= XI_HTTP_MAX_HEADERS
            , XI_HTTP_HEADER_PARSE_ERROR );

        // update the pointer
        header_end_ptr += sizeof( XI_HTTP_CRLF ) - 1;
    }

    return header_end_ptr;

err_handling:
    return 0;
}

http_response_t* parse_http( http_response_t* response, const char* content )
{
    memset( response, 0, sizeof( http_response_t ) );

    // remember where is the end of header section
    const char* ptr_to_headers_end = strstr( content, XI_HTTP_CRLFX2 );

    // check the continuation condition
    XI_CHECK_ZERO( ptr_to_headers_end, XI_HTTP_PARSE_ERROR );

    // update the pointer
    ptr_to_headers_end += sizeof( XI_HTTP_CRLF ) - 1;

    const char* payload_begin = ptr_to_headers_end + sizeof( XI_HTTP_CRLF ) - 1;

    XI_CHECK_ZERO( payload_begin, XI_HTTP_PARSE_ERROR );

    // parse status
    const char* ptr = parse_http_status( response, content );

    // check the continuation condition
    XI_CHECK_ZERO( ptr, XI_HTTP_PARSE_ERROR );

    // read the headers
    while( ( ptr = parse_http_header( response, ptr ) ) != '\0'
        && ptr != ptr_to_headers_end );

    // if there was an error, forward it
    xi_err_t e = xi_get_last_error();
    XI_CHECK_CND( e != XI_NO_ERR, e );

    // just copy the content
    strncpy( response->http_content, payload_begin, sizeof( response->http_content ) );

    return response;

err_handling:
    return 0;
}
