/*
 * hal_radio.cpp
 *
 *  Created on: 2014-2-24
 *      Author: cheng
 */
#include "hal_radio.h"

namespace mbed {

void cHalRadio::radio_io_init(int spirate)
{
	m_cs = 1;
	m_spi.format(8, 0);
	m_spi.frequency(spirate);
//	irq_pin.rise(this,&cHalRadio::rf_irq_callback);
	irq_pin.rise(&hal_rf230_ISR);

//	irq_pin.enable_irq();
}

cHalRadio::~cHalRadio()
{
}

cHalRadio::cHalRadio(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName slp, PinName irq)
:m_spi(mosi, miso, sclk), m_cs(cs), reset_pin(rst), sleep_pin(slp),irq_pin(irq)
{
	radio_io_init(4000000);
//	hal_init();
}


}
///* === irq ============================================================= */
//
//void cHalRadio::rf_irq_callback()
//{
//	hal_rf230_ISR();
//    //cli();
//    //EI_TRX_IRQ();
//}
