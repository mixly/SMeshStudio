/*
 * Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef MIST_DEFAULT_CONF_H
#define MIST_DEFAULT_CONF_H

#ifndef MIST_CONF_NETSTACK

#error MIST_CONST_NETSTACK must be #defined before #including mist-default-conf.h

#endif /* MIST_CONF_NETSTACK */

#if ((MIST_CONF_NETSTACK) & MIST_CONF_AES)
#include "netstack-aes.h" /* NETSTACK_AES_MICLEN = 4*/
#else /* ((MIST_CONF_NETSTACK) & MIST_CONF_AES) */
#define NETSTACK_AES_MICLEN 0
#endif /* ((MIST_CONF_NETSTACK) & MIST_CONF_AES) */


#if ((MIST_CONF_NETSTACK) & MIST_CONF_MULTICHAN)

/* Multichan-enabled configuration below */

#if ((MIST_CONF_NETSTACK) & MIST_CONF_CONTIKIMAC)

/* Multichan and ContikiMAC cannot be combined */

#error MIST_CONF_MULTICHAN cannot be used with MIST_CONF_CONTIKIMAC

#elif ((MIST_CONF_NETSTACK) & MIST_CONF_DROWSIE)

/* Multichan and Drowsie can be combined, by using the
   drowsie_multichannel code. */

#define DROWSIE_MAC_INPUT()   drowsie_multichannel_input()
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     drowsie_multichannel_driver
#define NETSTACK_RDC_HEADER_LEN 4
#else /* ((MIST_CONF_NETSTACK) & MIST_CONF_DROWSIE) */

/* Otherwise, we use the raw multichan driver */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     multichan_driver
#define NETSTACK_RDC_HEADER_LEN 4

#endif /* ((MIST_CONF_NETSTACK) & MIST_CONF_DROWSIE) */
#else /* ((MIST_CONF_NETSTACK) & MIST_CONF_MULTICHAN) */

/* No multichan configuration follows */

#if ((MIST_CONF_NETSTACK) & MIST_CONF_DROWSIE)

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     drowsie_driver
#define NETSTACK_RDC_HEADER_LEN 0

#elif ((MIST_CONF_NETSTACK) & MIST_CONF_CONTIKIMAC)

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     contikimac_driver
#define NETSTACK_RDC_HEADER_LEN 0

#else /* ((MIST_CONF_NETSTACK) & MIST_CONF_NULLRDC) */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver
#define NETSTACK_RDC_HEADER_LEN 0

#endif /* ((MIST_CONF_NETSTACK) & MIST_CONF_DROWSIE) */

#endif /* ((MIST_CONF_NETSTACK) & MIST_CONF_MULTICHAN) */


#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC csma_driver
#define NETSTACK_MAC_HEADER_LEN 0

#define SICSLOWPAN_CONF_MAC_MAX_PAYLOAD \
  (NETSTACK_RADIO_MAX_PAYLOAD_LEN - NETSTACK_MAC_HEADER_LEN - \
   NETSTACK_RDC_HEADER_LEN - NETSTACK_AES_MICLEN)


#endif /* MIST_DEFAULT_CONF_H */
