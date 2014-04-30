// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

/**
 * \file    xi_helpers.h
 * \author  Olgierd Humenczuk
 * \brief   General helpers used by the library
 */

#ifndef __XI_HELPERS_H__
#define __XI_HELPERS_H__

#include <stdlib.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \note    Needed to avoid using `strdup()` which can cause some problems with `free()`,
 *          because of buggy `realloc()` implementations.
 *
 * \return  Duplicated string or null in case of memory allocation problem.
 */
char* xi_str_dup( const char* s );

/**
 * \brief   Copies `src` string into `dst`, but stops whenever `delim` is reached or the `dst_size` is exceeded
 *
 * \return  Number of copied characters or -1 if an error occurred.
 */
int xi_str_copy_untiln( char* dst, size_t dst_size, const char* src, char delim );

/**
 * \brief   Replaces `p` with `r` for every `p` in `buffer`
 *
 * \return  Pointer to converted buffer.
 * \note    This function assumes that the buffer is terminated with `\0`.
 */
char* xi_replace_with(
      char p, char r
    , char* buffer
    , size_t max_chars );

#ifdef __cplusplus
}
#endif

#endif // __XI_HELPERS_H__
