/*
 * slip_uart.h
 *
 *  Created on: 2014-3-5
 *      Author: cheng
 */

#ifndef SLIP_UART_H_
#define SLIP_UART_H_

#include "dev/slip.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
//#include "slip_uart.h"
extern void slip_arch_init(unsigned long ubr);
//extern const struct uip_fallback_interface rpl_interface;
#define BAUD2UBR(x) x

/* Let slip know mist is here */
#endif /* SLIP_UART_H_ */
