// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file	csv_data_layer.c
 * \author  Olgierd Humenczuk
 * \brief 	Implements CSV _data layer_ abstration interface [see csv_data_layer.h and data_layer.h]
 */

#include "csv_data_layer.h"

const data_layer_t* get_csv_data_layer()
{
    static const data_layer_t __csv_data_layer = {
          csv_encode_datapoint
        , csv_encode_datapoint_in_place
        , csv_encode_create_datastream
        , csv_decode_feed
        , csv_decode_datapoint
    };

    return &__csv_data_layer;
}
