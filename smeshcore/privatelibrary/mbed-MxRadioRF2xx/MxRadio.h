/* Copyright (c) 2011 Frank Zhao
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

#ifndef MxRadio_h

#ifdef __cplusplus
extern "C" {
#endif
#include "uracolib/board.h"
#include "MxRadioCfg.h"
#include "uracolib/radio.h"
#include "uracolib/transceiver.h"
#include <stdint.h>
#ifdef __cplusplus
} /* extern "C" */
#endif

#include "mbed.h"
#define ZR_RXFRMBUFF_SIZE MAX_FRAME_SIZE
#define ZR_FIFO_SIZE 128 // size for the RX FIFO ring buffer
#define ZR_TXTMPBUFF_SIZE MAX_FRAME_SIZE // size for the TX non-immediate transmit buffer

#define ZR_TXWAIT_BEFORE // when you call any TX functions, it will wait until the previous transmission has finished before initiating a new transmission
#define ZR_TXWAIT_AFTER // when you call any TX functions, it will transmit and then wait until that transmission finished

// just a class definition, for usage and comments, see the cpp file
class cMxRadio
{
private:

	uint8_t temprssi;
	radio_status_t radiostatus;
	SPI m_spi;
	DigitalOut m_cs;
	DigitalOut reset_pin;
	DigitalOut sleep_pin;

protected:
	InterruptIn irq_pin;
private:
	 uint8_t rxFrameBuffer[ZR_RXFRMBUFF_SIZE];
	 uint8_t rxRingBuffer[ZR_FIFO_SIZE];
	 uint8_t rxRingBufferHead;
	 uint8_t rxRingBufferTail;
	 uint8_t txTmpBuffer[ZR_TXTMPBUFF_SIZE];
	 uint8_t txTmpBufferLength;
	 uint8_t lastLqi;
	 uint8_t lastRssi;
	 uint8_t hasAttachedRxEvent;
	 uint8_t hasAttachedTxEvent;
	 uint8_t usedBeginTransmission;
	volatile  uint8_t txIsBusy;

	 uint8_t* (*zrEventReceiveFrame)(uint8_t, uint8_t*, uint8_t, int8_t,uint8_t);
	 void (*zrEventTxDone)(radio_tx_done_t);

	 uint8_t* onReceiveFrame(uint8_t, uint8_t*, uint8_t, int8_t,uint8_t);
	 void onTxDone(radio_tx_done_t);
	 bool setautotx,setautorx,needack;

	//come from radio.h
	void radio_init(uint8_t * rxbuf, uint8_t rxbufsz);
	void radio_force_state(radio_state_t state);
	void radio_set_state(radio_state_t state);
	void radio_set_param(radio_attribute_t attr, radio_param_t parm);
	void radio_send_frame(uint8_t len, uint8_t *frm, uint8_t compcrc);
	radio_cca_t radio_do_cca(void);
	int radio_putchar(int c);
	int radio_getchar(void);
	void usr_radio_error(radio_error_t err);
	void usr_radio_irq(uint8_t cause);
	uint8_t * usr_radio_receive_frame(uint8_t len, uint8_t *frm, uint8_t lqi, int8_t ed, uint8_t crc_fail);
	void usr_radio_tx_done(radio_tx_done_t status);
	//come from transciever
	void trx_io_init (int spirate);
	void trx_reg_write(trx_regaddr_t addr, trx_regval_t val);
	uint8_t trx_reg_read(trx_regaddr_t addr);
	trx_regval_t trx_bit_read(trx_regaddr_t addr, trx_regval_t mask, uint8_t pos);
	void trx_bit_write(trx_regaddr_t addr, trx_regval_t mask, uint8_t pos, trx_regval_t value);
	void trx_frame_write(uint8_t length, uint8_t *data);
	uint8_t trx_frame_read(uint8_t *data, uint8_t datasz, uint8_t *lqi);
	//uint8_t trx_frame_read_crc(uint8_t *data, uint8_t datasz, bool *crc_ok);
	//uint8_t trx_frame_read_data_crc(uint8_t *data, uint8_t datasz, uint8_t *lqi, bool *crc_ok);
	uint8_t trx_frame_get_length(void);
	void trx_sram_write(trx_ramaddr_t addr, uint8_t length, uint8_t *data);
	void trx_sram_read(trx_ramaddr_t addr, uint8_t length, uint8_t *data);
	void trx_parms_get(trx_param_t *p);
	uint8_t trx_parms_set(trx_param_t *p);
	uint8_t trx_set_datarate(uint8_t rate_type);
	uint8_t trx_get_datarate(void);
	uint8_t trx_get_number_datarates(void);
	//void * trx_get_datarate_str_p(uint8_t idx);
	//void * trx_decode_datarate_p(uint8_t rhash);
	//uint8_t trx_get_datarate_str(uint8_t idx, char * rstr, uint8_t nlen);
	//uint8_t trx_decode_datarate(uint8_t rhash, char * rstr, uint8_t nlen);
	/**
	 * @brief Basic radio initialization function,
	 */
	 inline uint8_t trx_init(void)
	{
		uint8_t val;
		/* reset transceiver */
		reset_pin=0;//TRX_RESET_LOW();
		sleep_pin=0;//TRX_SLPTR_LOW();
		DELAY_US(TRX_RESET_TIME_US);
		reset_pin=1;//TRX_RESET_HIGH();
		/* set TRX_OFF (for the case we come from P_ON) */
		trx_reg_write(RG_TRX_STATE, CMD_TRX_OFF);

#if RADIO_TYPE == RADIO_AT86RF212
		trx_reg_write(RG_TRX_CTRL_0, 0x19);
#ifdef CHINABAND
		trx_reg_write(RG_CC_CTRL_1, CCBAND );
		trx_reg_write(RG_CC_CTRL_0, 11);//channel 0
		trx_reg_write(RG_TRX_CTRL_2, TRX_OQPSK250);
		DELAY_US(510);
#endif
#endif

		DELAY_US(TRX_INIT_TIME_US);
		val = trx_reg_read(RG_TRX_STATUS);
		return (val != TRX_OFF) ? TRX_OK : TRX_INIT_FAIL;
	}
	 inline uint8_t trx_check_pll_lock(void)
	{
	uint8_t val, cnt = 255;

	    trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
	    trx_reg_write(RG_IRQ_MASK, TRX_IRQ_PLL_LOCK);
	    trx_reg_write(RG_TRX_STATE, CMD_PLL_ON);
	    cnt = 255;
	    do
	    {
	        DELAY_US(TRX_PLL_LOCK_TIME_US);
	        val = trx_reg_read(RG_IRQ_STATUS);
	        if (val & TRX_IRQ_PLL_LOCK)
	        {
	            break;
	        }
	    }
	    while(--cnt);

	    /* clear pending IRQs*/
	    trx_reg_write(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
	    trx_reg_read(RG_IRQ_STATUS);
	    return (cnt > 0) ? TRX_OK : TRX_PLL_FAIL;
	}

	/**
	 * @brief Verify that correct radio type is used.
	 *
	 * @return status value, with the following meaning:
	 *        - 0 if part and revision number match
	 *        - 1 if revision number does @b not match
	 *        - 2 if part number does @b not match
	 *        - 3 if part and revision number does @b not match
	 */
	 inline int trx_identify(void)
	{
	    int ret = 0;

	    if(RADIO_PART_NUM != trx_reg_read(RG_PART_NUM))
	    {
	        ret |= INVALID_PART_NUM;
	    }

	    if(RADIO_VERSION_NUM != trx_reg_read(RG_VERSION_NUM))
	    {
	        ret |= INVALID_REV_NUM;
	    }
	    return ret;
	}

	/**
	 * @brief Write the PANID to the address filter registers
	 */
	 inline void trx_set_panid(uint16_t panid)
	{
	    trx_reg_write(RG_PAN_ID_0,(panid&0xff));
	    trx_reg_write(RG_PAN_ID_1,(panid>>8));
	}

	/**
	 * @brief Write the 16 bit short address to the
	 * address filter registers
	 */
	 inline void trx_set_shortaddr(uint16_t shortaddr)
	{
	    trx_reg_write(RG_SHORT_ADDR_0,(shortaddr&0xff));
	    trx_reg_write(RG_SHORT_ADDR_1,(shortaddr>>8));
	}

	/**
	 * @brief Write the 64 bit long address (MAC address) to the
	 * address filter registers
	 */
	 inline void trx_set_longaddr(uint64_t longaddr)
	{
	    trx_reg_write(RG_IEEE_ADDR_0, (uint8_t)(longaddr>>0) );
	    trx_reg_write(RG_IEEE_ADDR_1, (uint8_t)(longaddr>>8) );
	    trx_reg_write(RG_IEEE_ADDR_2, (uint8_t)(longaddr>>16));
	    trx_reg_write(RG_IEEE_ADDR_3, (uint8_t)(longaddr>>24));
	    trx_reg_write(RG_IEEE_ADDR_4, (uint8_t)(longaddr>>32));
	    trx_reg_write(RG_IEEE_ADDR_5, (uint8_t)(longaddr>>40));
	    trx_reg_write(RG_IEEE_ADDR_6, (uint8_t)(longaddr>>48));
	    trx_reg_write(RG_IEEE_ADDR_7, (uint8_t)(longaddr>>56));
	}

	 inline uint16_t crc16_update(uint16_t crc, uint8_t a)
	     {
	         int i;

	         crc ^= a;
	         for (i = 0; i < 8; ++i)
	         {
	             if (crc & 1)
	                 crc = (crc >> 1) ^ 0xA001;
	             else
	                 crc = (crc >> 1);
	         }

	         return crc;
	     }

	void rf_irq_callback();

	//rf230.cpp
	void radio_error(radio_error_t err);
	void radio_receive_frame(void);
	void radio_irq_handler(uint8_t cause);
	//events
	void zr_attach_error(void (*)(radio_error_t));
	void zr_attach_irq(void (*)(uint8_t));
	void zr_attach_receive_frame(uint8_t* (cMxRadio::*)(uint8_t, uint8_t*, uint8_t, int8_t,uint8_t));
	void zr_attach_tx_done(void (cMxRadio::*)(radio_tx_done_t));

	 void (*user_radio_error)(radio_error_t);
	 void (*user_radio_irq)(uint8_t);
	 uint8_t* (cMxRadio::*user_radio_receive_frame)(uint8_t, uint8_t*, uint8_t, int8_t,uint8_t);
	 void (cMxRadio::*user_radio_tx_done)(radio_tx_done_t);


public:
	cMxRadio(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName slp, PinName irq);
	~cMxRadio();
	void begin(channel_t);
	void begin(channel_t,uint16_t,uint16_t,bool,bool,bool,char);
	void begin(channel_t,uint16_t,uint16_t,bool,bool,bool);
	void begin(channel_t, uint8_t*);
	void setFrameHeader(uint8_t*);
	void attachError(void(*)(radio_error_t));
	void attachIrq(void(*)(uint8_t));
	void attachReceiveFrame(uint8_t* (*)(uint8_t, uint8_t*, uint8_t, int8_t,uint8_t));
	void sendFrame(uint16_t ,bool ,uint8_t* , uint8_t );
	void attachTxDone(void(*)(radio_tx_done_t));
	int8_t available();
	int16_t peek();
	int16_t read();
	void flush();
	void write(uint8_t);
	void write(char*);
	void write(uint8_t*, uint8_t);
	void send(uint8_t);
	void txFrame(uint8_t*, uint8_t);
	void beginTransmission();
	void beginTransmission(uint16_t);
	void endTransmission();
	void cancelTransmission();
	void setParam(radio_attribute_t, radio_param_t);
	radio_cca_t doCca();
	void setState(radio_state_t, uint8_t);
	void setState(radio_state_t);
	void setStateRx();
	void setChannel(channel_t);
	uint8_t getChannel();
	void forceState(radio_state_t);
	void waitTxDone(uint16_t);
	int8_t getRssiNow();
	int8_t getLastRssi();
	uint8_t getLqi();
	int8_t getLastEd();
	int8_t getEdNow();
};

extern cMxRadio MxRadio; // make single instance accessible

#define MxRadio_h
#endif
