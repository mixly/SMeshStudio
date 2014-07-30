/*
 * radio_api.h
 *
 *  Created on: 2014-2-24
 *      Author: cheng
 */

#ifndef RADIO_API_H_
#define RADIO_API_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
//#define hal_set_rst_high( )   ( PORT_RST |= ( 1 << RST ) )  /**< This macro pulls the RST pin high. */
//#define hal_set_rst_low( )    ( PORT_RST &= ~( 1 << RST ) ) /**< This macro pulls the RST pin low. */
//#define hal_get_rst( )        ( ( PIN_RST & ( 1 << RST )  ) >> RST )  /**< Read current state of the RST pin (High/Low). */
//#define hal_set_slptr_high( ) ( PORT_SLP_TR |= ( 1 << SLP_TR ) )      /**< This macro pulls the SLP_TR pin high. */
//#define hal_set_slptr_low( )  ( PORT_SLP_TR &= ~( 1 << SLP_TR ) )     /**< This macro pulls the SLP_TR pin low. */
//#define hal_get_slptr( )      ( PIN_SLP_TR & ( 1 << SLP_TR ) )   /**< Read current state of the SLP_TR pin (High/Low). */

#define HAL_BAT_LOW_MASK       ( 0x80 ) /**< Mask for the BAT_LOW interrupt. */
#define HAL_TRX_UR_MASK        ( 0x40 ) /**< Mask for the TRX_UR interrupt. */
#define HAL_TRX_END_MASK       ( 0x08 ) /**< Mask for the TRX_END interrupt. */
#define HAL_RX_START_MASK      ( 0x04 ) /**< Mask for the RX_START interrupt. */
#define HAL_PLL_UNLOCK_MASK    ( 0x02 ) /**< Mask for the PLL_UNLOCK interrupt. */
#define HAL_PLL_LOCK_MASK      ( 0x01 ) /**< Mask for the PLL_LOCK interrupt. */

#define HAL_MIN_FRAME_LENGTH   ( 0x03 ) /**< A frame should be at least 3 bytes. */
#define HAL_MAX_FRAME_LENGTH   ( 0x7F ) /**< A frame should no more than 127 bytes. */

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

/** This macro will protect the following code from interrupts.*/
#define HAL_ENTER_CRITICAL_REGION() __disable_irq()

/** This macro must always be used in conjunction with HAL_ENTER_CRITICAL_REGION
    so that interrupts are enabled again.*/
#define HAL_LEAVE_CRITICAL_REGION()  __enable_irq()

/** \struct hal_rx_frame_t
 *  \brief  This struct defines the rx data container.
 *
 *  \see hal_frame_read
 */
typedef struct{
    uint8_t length;                       /**< Length of frame. */
    uint8_t data[ HAL_MAX_FRAME_LENGTH ]; /**< Actual frame data. */
    uint8_t lqi;                          /**< LQI value for received frame. */
    bool crc;                             /**< Flag - did CRC pass for received frame? */
} hal_rx_frame_t;

void hal_init(void);
void hal_rf230_ISR();
uint8_t hal_register_read(uint8_t address);
void hal_register_write(uint8_t address, uint8_t value);
uint8_t hal_subregister_read( uint8_t address, uint8_t mask, uint8_t position );
void hal_subregister_write( uint8_t address, uint8_t mask, uint8_t position,
                            uint8_t value );

void hal_frame_read(hal_rx_frame_t *rx_frame);
void hal_frame_write( uint8_t *write_buffer, uint8_t length );
void hal_sram_read( uint8_t address, uint8_t length, uint8_t *data );
void hal_sram_write( uint8_t address, uint8_t length, uint8_t *data );

/* Number of receive buffers in RAM. */
#ifndef RF230_CONF_RX_BUFFERS
#define RF230_CONF_RX_BUFFERS 1
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RADIO_API_H_ */
