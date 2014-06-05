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
 * @brief Interface for @ref grpLibRadio.
 *
 */
#ifndef RADIO_H
#define RADIO_H

/* === Includes ============================================================== */
#include <stdbool.h>
#include <stdio.h>
#include "const.h"

/**
 * @addtogroup grpRadio
 * @{
 */

/* === Externals ============================================================= */

/* === Types ================================================================= */

/**
 * Radio state enumeration
 */

/** TRX in OFF state. */
#define  STATE_OFF (0)
/** Basic mode TX state. */
#define  STATE_TX (1)
/** Basic mode RX state. */
#define STATE_RX (2)
/** Extended mode TX state (TX_ARET). */
#define STATE_TXAUTO (3)
/** Extended mode RX state (RX_AACK). */
#define STATE_RXAUTO (4)
/** Sleep state (lowest power consumption). */
#define STATE_SLEEP (5)


/** Radio state type */
typedef uint8_t radio_state_t;

/**
 * error codes for tx done event
 */
typedef enum
{
    TX_OK,			/**< transmission completed successfully */
    TX_CCA_FAIL,		/**< channel was busy (TX_AUTO only) */
    TX_NO_ACK,			/**< no ACK received (TX_AUTO only) */
    TX_FAIL,			/**< unexpected error */
} radio_tx_done_t;


/**
 * codes for CCA
 */
typedef enum SHORTENUM
{
#if defined(CCA_BUSY)
	/** @todo this is workaround for name clash for RFA1 and avr-libc
	 *   avr/iom128rfa1.h, check if this is OK or if we should change
	 *   the names of CCA_FREE, ..., etc.
	 */
	RADIO_CCA_FREE = CCA_IDLE,
	RADIO_CCA_BUSY = CCA_BUSY,
	RADIO_CCA_FAIL = 255,
#else
    /** The CCA measurement estimates, that the channel is free. */
    RADIO_CCA_FREE = 0,
    /** The CCA measurement estimates, that the channel is busy. */
    RADIO_CCA_BUSY,
    /** The CCA measurement was not finished. */
    RADIO_CCA_FAIL
#endif

} radio_cca_t;


/** Enumeration to identify radio attributes.
 */
typedef enum SHORTENUM
{
    /** Set the current channel */
    phyCurrentChannel,
    /** Currently unused */
    phyChannelsSupported,
    /** Set the Tx power */
    phyTransmitPower,
    /** Transceiver state to return to after transmission */
    phyIdleState,
    /** CCA mode to use in CSMA-CA:
     * <table>
     *   <tr><th>value</th><th>CCA mode</th></tr>
     *   <tr><td>0</td>    <td>carrier sense OR energy above threshold</td></tr>
     *   <tr><td>1</td>    <td>energy above threshold (default)</td></tr>
     *   <tr><td>2</td>    <td>carrier sense</td></tr>
     *   <tr><td>3</td>    <td>carrier sense AND energy above threshold</td></tr>
     * </table>
     */
    phyCCAMode,
    /** PAN ID to use in STATE_RXAUTO frame filter */
    phyPanId,
    /** Short (16-bit) address to use in STATE_RXAUTO frame filter */
    phyShortAddr,
    /** Pointer to long (EUI-64) address to use in STATE_RXAUTO frame filter */
    phyLongAddr,

    /** Datarate */
    phyDataRate,

    /** PA enable */
    phyTxPa,
    /** LNA enable */
    phyRxLna

} radio_attribute_t;


/**
 * @brief Container for handover of radio parameter values.
 *
 * @note
 * The elements in this union should have max. a size
 * of 2 byte, all other parameters should be configured with
 * the void pointer.
 */
typedef union radio_param_t
{
#if defined __cplusplus
public:
    radio_param_t(int8_t c) { channel = c; } /* also used for txpwr_t */
    //radio_param_t(radio_state_t s) { idle_state = s; }
    radio_param_t(uint8_t m) { cca_mode = m; } /* also used for data_rate, tx_pa, rx_lna */
    radio_param_t(uint16_t p) { pan_id = p; } /* also used for short_addr */
    radio_param_t(uint64_t *la) { long_addr = la; }
#endif
    /** Value for current radio channel. (@ref MIN_CHANNEL ... @ref MAX_CHANNEL)*/
    channel_t channel;
    /** Value for transmit power in dB.*/
    txpwr_t   tx_pwr;
    /** after TX go to idle state */
    radio_state_t idle_state;
    /** Value for cca mode. */
    ccamode_t cca_mode;
    /** Value for PANID */
    uint16_t pan_id;
    /** Value for short address */
    uint16_t short_addr;
    /** Pointer to long (64-bit) address */
    uint64_t *long_addr;
    /** data rate type */
    uint8_t data_rate;

    /** TX power amp type */
    uint8_t tx_pa;
    /** RX LNA type */
    uint8_t rx_lna;

} radio_param_t;


/**
 * @brief Error codes.
 */
typedef enum SHORTENUM
{
	#ifndef SUCCESS
	/** @todo same like CCA_FREE, include/avr/iom128rfa1.h:3687:#define SUCCESS*/
    SUCCESS = 0,        /**< OK Code*/
    #endif
    STATE_SET_FAILED = 1,   /**< function radio_set_state failed */
    SET_PARM_FAILED,    /**< function radio_set_param failed */
    GET_PARM_FAILED,    /**< function radio_get_param failed */
    GENERAL_ERROR,      /**< something unexpected happened */
} radio_error_t;


/**
 * @brief Structure for storage of radio parameters.
 */
typedef struct
{
    uint8_t  channel;      /**< Current radio channel.
                                (@ref MIN_CHANNEL ... @ref MAX_CHANNEL) */
    uint8_t  tx_pwr;       /**< Current transmit power. */
    uint8_t  cca_mode;     /**< Current cca mode. */
    radio_state_t state;      /**< Current transceiver state. */
    radio_state_t idle_state; /**< after TX go to idle state */
    uint8_t  *rxframe;      /**< Pointer for frame data storage. */
    uint8_t  rxframesz;     /**< Length of the buffer rxframesz */
    uint8_t tx_pa;
    uint8_t rx_lna;
} radio_status_t;

/* === Macros ================================================================ */

/**
 * Code for invalid RSSI value.
 */
#define VOID_RSSI (0xff)

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the channel number to @c x.
 */
#  define RP_CHANNEL(x)
#elif defined __cplusplus
#  define RP_CHANNEL(x) phyCurrentChannel,radio_param_t((channel_t)x)
#else
#  define RP_CHANNEL(x) phyCurrentChannel,(radio_param_t){.channel=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the tx power value to @c x.
 */
#  define RP_TXPWR(x)
#elif defined __cplusplus
#  define RP_TXPWR(x)   phyTransmitPower,radio_param_t((txpwr_t)x)
#else
#  define RP_TXPWR(x)   phyTransmitPower,(radio_param_t){.tx_pwr=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the transceiver's idle state to @c x.
 */
#  define RP_IDLESTATE(x)
#elif defined __cplusplus
#  define RP_IDLESTATE(x)  phyIdleState,radio_param_t(x)
#else
#  define RP_IDLESTATE(x)  phyIdleState,(radio_param_t){.idle_state=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the CCA mode to @c x.
 */
#  define RP_CCAMODE(x)
#elif defined __cplusplus
#  define RP_CCAMODE(x)  phyCCAMode,radio_param_t((ccamode_t)x)
#else
#  define RP_CCAMODE(x) phyCCAMode,(radio_param_t){.cca_mode=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the PAN ID to @c x.
 */
#  define RP_PANID(x)
#elif defined __cplusplus
#  define RP_PANID(x) phyPanId,radio_param_t((uint16_t)x)
#else
#  define RP_PANID(x) phyPanId,(radio_param_t){.pan_id=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the short address to @c x.
 */
#  define RP_SHORTADDR(x)
#elif defined __cplusplus
#  define RP_SHORTADDR(x) phyShortAddr,radio_param_t((uint16_t)x)
#else
#  define RP_SHORTADDR(x) phyShortAddr,(radio_param_t){.short_addr=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the long address pointer to @c x.
 */
#  define RP_LONGADDR(x)
#elif defined __cplusplus
#  define RP_LONGADDR(x) phyLongAddr,radio_param_t((uint64_t *)x)
#else
#  define RP_LONGADDR(x) phyLongAddr,(radio_param_t){.long_addr=x}
#endif


#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the data rate to @c x.
 */
#  define RP_DATARATE(x)
#elif defined __cplusplus
#  define RP_DATARATE(x) phyDataRate,radio_param_t((uint16_t)x)
#else
#  define RP_DATARATE(x) phyDataRate,(radio_param_t){.data_rate=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the TX power amplifier enable number to @c x.
 */
#  define RP_CHANNEL(x)
#elif defined __cplusplus
#  define RP_TX_PA(x) phyTxPa,radio_param_t((tx_pa_t)x)
#else
#  define RP_TX_PA(x) phyTxPa,(radio_param_t){.tx_pa=x}
#endif

#if defined(DOXYGEN)
/**
 * Helper macro to construct the arguments for @ref radio_set_param in
 * order to set the TX power amplifier enable number to @c x.
 */
#  define RP_RX_LNA(x)
#elif defined __cplusplus
#  define RP_RX_LNA(x) phyRxLna,radio_param_t((rx_lna_t)x)
#else
#  define RP_RX_LNA(x) phyRxLna,(radio_param_t){.rx_lna=x}
#endif

#define CRC_CCITT_UPDATE(crc, data) crc16_update(crc, data)


#ifndef RADIO_CFG_EEOFFSET
/** offset of radio config data in EEPROM */
#define RADIO_CFG_EEOFFSET (8)
#endif

#ifndef RADIO_CFG_DATA
/** a default radio configuration data structure */
#define RADIO_CFG_DATA {chan: 16, txp: 0, cca: 1, edt: 11, clkm: 0, crc: 0xab12}
#endif

/** @} */


#endif  /* #ifndef RADIO_H */
/* EOF */
