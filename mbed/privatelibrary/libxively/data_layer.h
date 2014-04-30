// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    data_layer.h
 * \author  Olgierd Humenczuk
 * \brief   Defines _data layer_ abstraction interface
 */

#ifndef __DATA_LAYER_H__
#define __DATA_LAYER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   _The data layer interface_ - contains function pointers,
 *          that's what we expose to the layers above and below
 *
 *    It is effectively a class that holds declarations of pure virtual functions.
 *    * All encoders take a given data type (e.g. `xi_feed_t`, `xi_datapoint_t`) and produce
 *    an encoded string in a buffer, which can be wrapped as payload to _transport layer_
 *    or any other layer, such as _gzip_.
 *    * All decoders take a given data buffer and convert to an appropriate data type.
 *
 * \note    The encoders and decoders do not have to be paired, e.g. `encode_feed` is actually
 *          implemented using `encode_datapoint_in_place` in a loop (see `http_encode_update_feed`),
 *          so we only need `decode_feed`. However, this presumption had been made with CSV _data_
 *          _layer_ implementation and the interface may need to change if other data formats are
 *          to be implemented, but it is currently more important to have it the way it is.
 *          The layering model should allow for simple way of integrating anything, even if there
 *          might seem to be some inconsistency right now.
 */
typedef struct {
    /**
     * \brief   This function converts `xi_datapoint_t` into an implementation-specific format
     *          for a datapoint.
     *
     * \return  Pointer to the buffer where encoded string resides.
     */
    const char* ( *encode_datapoint )( const xi_datapoint_t* dp );

    /**
     * \brief   This function converts `xi_datapoint_t` into an implementation-specific format
     *          for a datapoint with output buffer given as an argument.
     *
     * \return  Offset or -1 if an error occurred.
     */
    int ( *encode_datapoint_in_place )(
          char* buffer, size_t buffer_size
        , const xi_datapoint_t* dp );

    /**
     * \brief   This function converts `xi_datapoint_t` and a given datastream ID string
     *          into an implementation-specific format for creating datastreams.

     * \return  Pointer to the buffer where encoded string resides.
     */
    const char* ( *encode_create_datastream )(
          const char* data
        , const xi_datapoint_t* dp );

    /**
     * \brief   This function converts from an implementation-specific format for a feed
     *          into `xi_feed_t` that is given as an argument.
     *
     * \return  Pointer to feed structure or null if an error occurred.
     */
    xi_feed_t* ( *decode_feed )( const char* data, xi_feed_t* feed );

    /**
     * \brief   This function converts from an implementation-specific format for a datapoint
     *          into `xi_datapoint_t` that is given as an argument.
     *
     * \return  Pointer to datastream structure or null if an error occurred.

     */
    xi_datapoint_t* ( *decode_datapoint )( const char* data, xi_datapoint_t* dp );
} data_layer_t;

#ifdef __cplusplus
}
#endif

#endif // __DATA_LAYER_H__
