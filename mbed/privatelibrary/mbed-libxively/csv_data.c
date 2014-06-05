// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    csv_data.c
 * \author  Olgierd Humenczuk
 * \brief   Implements CSV _data layer_ encoders and decoders specific to Xively CSV data format [see csv_data.h]
 */

#include <stdlib.h>
#include <stdio.h>

#include "csv_data.h"
#include "xi_macros.h"
#include "xi_helpers.h"
#include "xi_debug.h"
#include "xi_err.h"
#include "xi_config.h"

static char XI_CSV_LOCAL_BUFFER[ XI_CSV_BUFFER_SIZE ];

inline static int csv_encode_value(
      char* buffer
    , size_t buffer_size
    , const xi_datapoint_t* p )
{
    // PRECONDITION
    assert( buffer != 0 );
    assert( buffer_size != 0 );
    assert( p != 0 );

    switch( p->value_type )
    {
        case XI_VALUE_TYPE_I32:
            return snprintf( buffer, buffer_size, "%d", p->value.i32_value );
        case XI_VALUE_TYPE_F32:
            return snprintf( buffer, buffer_size, "%f", p->value.f32_value );
        case XI_VALUE_TYPE_STR:
            return snprintf( buffer, buffer_size, "%s", p->value.str_value );
        default:
            return -1;
    }
}

typedef enum
{
    XI_CHAR_UNKNOWN = 0,
    XI_CHAR_NUMBER,
    XI_CHAR_LETTER,
    XI_CHAR_DOT,
    XI_CHAR_SPACE,
    XI_CHAR_NEWLINE,
    XI_CHAR_TAB,
    XI_CHAR_MINUS
} xi_char_type_t;

inline static xi_char_type_t csv_classify_char( char c )
{
    switch( c )
    {
        case 13:
        case 11:
            return XI_CHAR_NEWLINE;
        case 9:
            return XI_CHAR_TAB;
        case 32:
            return XI_CHAR_SPACE;
        case 33: case 34: case 35: case 36: case 37: case 38: case 39:
        case 40: case 41: case 42: case 43: case 44:
            return XI_CHAR_UNKNOWN;
        case 45:
            return XI_CHAR_MINUS;
        case 46:
            return XI_CHAR_DOT;
        case 47:
            return XI_CHAR_UNKNOWN;
        case 48: case 49: case 50: case 51: case 52: case 53: case 54:
        case 55: case 56:
        case 57:
            return XI_CHAR_NUMBER;
        case 58: case 59: case 60: case 61: case 62: case 63:
        case 64:
            return XI_CHAR_UNKNOWN;
        case 65: case 66: case 67: case 68: case 69: case 70: case 71:
        case 72: case 73: case 74: case 75: case 76: case 77: case 78:
        case 79: case 80: case 81: case 82: case 83: case 84: case 85:
        case 86: case 87: case 88: case 89:
        case 90:
            return XI_CHAR_LETTER;
        case 91: case 92: case 93: case 94: case 95:
        case 96:
            return XI_CHAR_UNKNOWN;
        case 97: case 98: case 99: case 100: case 101: case 102: case 103:
        case 104: case 105: case 106: case 107: case 108: case 109: case 110:
        case 111: case 112: case 113: case 114: case 115: case 116: case 117:
        case 118: case 119: case 120: case 121:
        case 122:
            return XI_CHAR_LETTER;
        case 123:
        case 124:
        case 125:
            return XI_CHAR_UNKNOWN;
        default:
            return XI_CHAR_UNKNOWN;
    }
}

typedef enum
{
    XI_STATE_INITIAL = 0,
    XI_STATE_MINUS,
    XI_STATE_NUMBER,
    XI_STATE_FLOAT,
    XI_STATE_DOT,
    XI_STATE_STRING
} xi_dfa_state_t;

xi_datapoint_t* csv_decode_value(
    const char* buffer, xi_datapoint_t* p )
{
    // PRECONDITION
    assert( buffer != 0 );
    assert( p != 0 );

    // secure the output buffer
    XI_GUARD_EOS( p->value.str_value, XI_VALUE_STRING_MAX_SIZE );

    // clean the counter
    size_t  counter = 0;

    // the transition function
    static const short states[][6][2] =
    {
          // state initial                          // state minus                            // state number                           // state float                            // state dot                              // string
        { { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  } },
        { { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_FLOAT   }, { XI_CHAR_NUMBER    , XI_STATE_FLOAT   }, { XI_CHAR_NUMBER    , XI_STATE_STRING  } },
        { { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  } },
        { { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_STRING  }, { XI_CHAR_DOT       , XI_STATE_STRING  }, { XI_CHAR_DOT       , XI_STATE_STRING  } },
        { { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  } },
        { { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL } },
        { { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  } },
        { { XI_CHAR_MINUS     , XI_STATE_MINUS   }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  } }
    };

    char    c = *buffer;
    short   s = XI_STATE_INITIAL;

    while( c != '\n' && c !='\0' && c!='\r' )
    {
        if( counter >= XI_VALUE_STRING_MAX_SIZE - 1 )
        {
            xi_set_err( XI_DATAPOINT_VALUE_BUFFER_OVERFLOW );
            return 0;
        }

        xi_char_type_t ct = csv_classify_char( c );
        s = states[ ct ][ s ][ 1 ];

        switch( s )
        {
            case XI_STATE_MINUS:
            case XI_STATE_NUMBER:
            case XI_STATE_FLOAT:
            case XI_STATE_DOT:
            case XI_STATE_STRING:
                p->value.str_value[ counter ] = c;
                break;
        }

        c = *( ++buffer );
        ++counter;
    }

    // set the guard
    p->value.str_value[ counter ] = '\0';

    // update of the state for loose states...
    switch( s )
    {
        case XI_STATE_MINUS:
        case XI_STATE_DOT:
        case XI_STATE_INITIAL:
            s = XI_STATE_STRING;
            break;
    }

    switch( s )
    {
        case XI_STATE_NUMBER:
            p->value.i32_value  = atoi( p->value.str_value );
            p->value_type       = XI_VALUE_TYPE_I32;
            break;
        case XI_STATE_FLOAT:
            p->value.f32_value  = ( float ) atof( p->value.str_value );
            p->value_type       = XI_VALUE_TYPE_F32;
            break;
        case XI_STATE_STRING:
        default:
            p->value_type       = XI_VALUE_TYPE_STR;
    }

    return p;
}

const char* csv_encode_datapoint( const xi_datapoint_t* data )
{

    // PRECONDITIONS
    assert( data != 0 );

    return csv_encode_datapoint_in_place( XI_CSV_LOCAL_BUFFER, sizeof( XI_CSV_LOCAL_BUFFER ), data ) == -1 ? 0 : XI_CSV_LOCAL_BUFFER;
}

int csv_encode_datapoint_in_place(
      char* in, size_t in_size
    , const xi_datapoint_t* datapoint )
{
    // PRECONDITIONS
    assert( in != 0 );
    assert( datapoint != 0 );

    int s       = 0;
    int size    = in_size;
    int offset  = 0;

    if( datapoint->timestamp.timestamp != 0 )
    {
        xi_time_t stamp = datapoint->timestamp.timestamp;
        struct xi_tm* gmtinfo = xi_gmtime( &stamp );

        s = snprintf( in, size
            , "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ,"
            , gmtinfo->tm_year + 1900
            , gmtinfo->tm_mon + 1
            , gmtinfo->tm_mday
            , gmtinfo->tm_hour
            , gmtinfo->tm_min
            , gmtinfo->tm_sec
            , ( int ) datapoint->timestamp.micro );

        XI_CHECK_S( s, size, offset, XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN );
    }

    s = csv_encode_value( in + offset, size - offset, datapoint );
    XI_CHECK_S( s, size, offset, XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN );

    s = snprintf( in + offset, size - offset, "%s", "\n" );
    XI_CHECK_S( s, size, offset, XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN );

    return offset;

err_handling:
    return -1;
}

const char* csv_encode_create_datastream(
          const char* datastream_id
        , const xi_datapoint_t* data )
{
    // PRECONDITIONS
    assert( data != 0 );

    int s       = 0;
    int size    = sizeof( XI_CSV_LOCAL_BUFFER );
    int offset  = 0;

    s = snprintf( XI_CSV_LOCAL_BUFFER + offset, size - offset
            , "%s,", datastream_id );
    XI_CHECK_S( s, size, offset, XI_CSV_ENCODE_DATASTREAM_BUFFER_OVERRUN );

    s = csv_encode_datapoint_in_place( XI_CSV_LOCAL_BUFFER + offset, size - offset, data );
    XI_CHECK_S( s, size, offset, XI_CSV_ENCODE_DATASTREAM_BUFFER_OVERRUN );

    return XI_CSV_LOCAL_BUFFER;

err_handling:
    return 0;
}

xi_feed_t* csv_decode_feed(
      const char* buffer
    , xi_feed_t* feed )
{
    const char* current     = buffer;
    int32_t counter         = 0;

    // find occurence of newline
    char* end_of_line = ( char* ) 1;

    // while we didn't jump out of the buffer
    while( end_of_line )
    {
        // get current datapoint
        xi_datastream_t* d    = &feed->datastreams[ counter ];
        d->datapoint_count    = 0;
        xi_datapoint_t* p     = &d->datapoints[ 0 ];
        memset( p, 0, sizeof( xi_datapoint_t ) );

        end_of_line = strstr( current, "\n" );

        const char* end_of_datastream_id  = strstr( current, "," );
        const char* beg_of_datapoint      = end_of_datastream_id + 1;

        XI_CHECK_ZERO( beg_of_datapoint, XI_CSV_DECODE_FEED_PARSER_ERROR );
        XI_CHECK_ZERO( end_of_datastream_id, XI_CSV_DECODE_FEED_PARSER_ERROR );

        int size = sizeof( d->datastream_id );

        int s = xi_str_copy_untiln( d->datastream_id, size
            , current, ',' );
        XI_CHECK_SIZE( s, size, XI_CSV_DECODE_FEED_PARSER_ERROR );

        xi_datapoint_t* ret = csv_decode_datapoint( beg_of_datapoint, p );
        XI_CHECK_ZERO( ret, XI_CSV_DECODE_FEED_PARSER_ERROR )

        d->datapoint_count = 1;
        current = end_of_line + 1;

        XI_CHECK_CND( ++counter == XI_MAX_DATASTREAMS
            , XI_CSV_DECODE_FEED_PARSER_ERROR );
    }

    feed->datastream_count = counter;
    return feed;

err_handling:
    return 0;
}

xi_datapoint_t* csv_decode_datapoint(
      const char* buffer
    , xi_datapoint_t* datapoint )
{
    // PRECONDITIONS
    assert( buffer != 0 );
    assert( datapoint != 0 );

    int ye, mo, da, h, m, s, ms;

    const char* beg_of_value = strstr( buffer, "," );

    // check continuation condition
    XI_CHECK_ZERO( beg_of_value, XI_CSV_DECODE_DATAPOINT_PARSER_ERROR );

    // move to pointer to the proper position
    beg_of_value += 1;

    {
        int n = sscanf( buffer
            , "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ"
            , &ye, &mo, &da, &h, &m, &s, &ms );

        // check if the parser worked correctly
        XI_CHECK_CND( n != 7, XI_CSV_DECODE_DATAPOINT_PARSER_ERROR );

        // copy parsed data
        {
            struct xi_tm timeinfo;

            timeinfo.tm_year   = ye - 1900;
            timeinfo.tm_mon    = mo - 1;
            timeinfo.tm_mday   = da;
            timeinfo.tm_hour   = h;
            timeinfo.tm_min    = m;
            timeinfo.tm_sec    = s;

            xi_time_t timestamp = xi_mktime( &timeinfo );

            XI_CHECK_CND( ( int )timestamp == -1, XI_CSV_TIME_CONVERTION_ERROR );

            datapoint->timestamp.timestamp  = timestamp;
            datapoint->timestamp.micro      = ms;
        }
    }

    xi_datapoint_t* r = csv_decode_value( beg_of_value
        , datapoint );


    XI_CHECK_ZERO( r, XI_CSV_DECODE_DATAPOINT_PARSER_ERROR );

    return datapoint;

err_handling:
    return 0;
}
