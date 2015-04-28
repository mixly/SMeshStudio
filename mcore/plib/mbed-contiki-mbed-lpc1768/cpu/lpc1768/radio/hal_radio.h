/*
 * hal_radio.h
 *
 *  Created on: 2014-2-24
 *      Author: cheng
 */

#ifndef HAL_RADIO_H_
#define HAL_RADIO_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "radio_api.h"
#include "radio_platform.h"
#ifdef __cplusplus
} /* extern "C" */
#endif
#include "SPI.h"
#include "platform.h"
#include "mbed.h"

namespace mbed {

/** A SPI Master, used for communicating with SPI slave devices
 *
 * The default format is set to 8-bits, mode 0, and a clock frequency of 1MHz
 *
 * Most SPI devices will also require Chip Select and Reset signals. These
 * can be controlled using <DigitalOut> pins
 *
 * Example:
 * @code
 * // Send a byte to a SPI slave, and record the response
 *
 * #include "mbed.h"
 *
 * SPI device(p5, p6, p7); // mosi, miso, sclk
 *
 * int main() {
 *     int response = device.write(0xFF);
 * }
 * @endcode
 */
class cHalRadio{

protected:
	InterruptIn irq_pin;
private:
	void radio_io_init(int spirate);
	void rf_irq_callback(void);
public:
	cHalRadio(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName rst, PinName slp, PinName irq);
	~cHalRadio();

	SPI m_spi;
	DigitalOut m_cs;
	DigitalOut reset_pin;
	DigitalOut sleep_pin;
};


} // namespace mbed
extern void rf230_set_channel(uint8_t c);
extern cHalRadio HalRadio;//(P0_18, P0_17, P0_15, P0_20, P2_11,P2_12, P2_13);//spi for radio;

#endif /* HAL_RADIO_H_ */
