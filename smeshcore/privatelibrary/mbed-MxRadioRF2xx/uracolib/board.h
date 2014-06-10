/* Copyright (c) 2007-2009 Axel Wachtler
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of the authors nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE. */

/* $Id$ */
/**
 * @file
 * @brief Interface for @ref grpBoard.
 * @ingroup grpBoard
 */
#ifndef BOARD_H
#define BOARD_H (1)
#include "const.h"
#include "../MxRadioCfg.h"
#define DELAY_US(x)  wait_us(x)
#define DELAY_MS(x)  wait_ms(x)



#if RADIO_TYPE == RADIO_AT86RF230 || defined(DOXYGEN)
# include "at86rf230a.h"
#elif RADIO_TYPE == RADIO_AT86RF230B
# include "at86rf230b.h"
#elif RADIO_TYPE == RADIO_AT86RF231
# include "at86rf231.h"
#elif RADIO_TYPE == RADIO_AT86RF212
# include "at86rf212.h"
#elif RADIO_TYPE == RADIO_AT86RF232
# include "at86rf232.h"
#elif RADIO_TYPE == RADIO_AT86RF233
# include "at86rf233.h"

#else
# error "RADIO_TYPE is not defined or wrong"
#endif

//# include "at86rf231.h"

#endif /* #ifndef BOARD_H */
