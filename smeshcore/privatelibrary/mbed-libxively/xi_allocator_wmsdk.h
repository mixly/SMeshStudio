// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file	xi_allocator_wmsdk.h
 * \author  Olgierd Humenczuk
 * \brief   Our custom `alloc()` and `free()` [see xi_allocator.h]
 */

#ifndef __XI_ALLOCATOR_STDLIB_H__
#define __XI_ALLOCATOR_STDLIB_H__

#include <wm_os.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void* xi_alloc( size_t b )
{
    return ( void* ) os_mem_alloc( b );
}

static inline void xi_free( void* p )
{
    os_mem_free( p );
}

#ifdef __cplusplus
}
#endif

#endif
