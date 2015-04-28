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

#ifdef __cplusplus
extern "C" {
#endif
#include "contiki.h"
#include "dp83848-ip64-driver.h"

#include "ip64.h"
#include "ip64-eth.h"

#include "rime/rime.h"

#include <string.h>
#ifdef __cplusplus
}
#endif
#include "mbed.h"

Ethernet eth;

PROCESS(dp83848_ip64_driver_process, "dp83848 IP64 driver");


/*---------------------------------------------------------------------------*/
static void
init(void)
{
  uint8_t eui64[8];
  uint8_t macaddr[6];

  /* Assume that rimeaddr_node_addr holds the EUI64 of this device. */
  //memcpy(eui64, &rimeaddr_node_addr, sizeof(eui64)); error

  /* Mangle the EUI64 into a 48-bit Ethernet address. */
  memcpy(&macaddr[0], &eui64[0], 3);
  memcpy(&macaddr[3], &eui64[5], 3);

  /* In case the OUI happens to contain a broadcast bit, we mask that
	 out here. */
  macaddr[0] = (macaddr[0] & 0xfe);

  memcpy(ip64_eth_addr.addr, macaddr, sizeof(macaddr));

//  dp83848_init(macaddr);
  printf("Wait connnect...\n");
  while(!eth.link());
  printf("Connected now!\n");

  process_start(&dp83848_ip64_driver_process, NULL);
}
/*---------------------------------------------------------------------------*/
static int
output(uint8_t *packet, uint16_t len)
{
//  printf("dp83848:output\n");
  eth.write((char *)packet, (int)len);
  eth.send();
  return len;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dp83848_ip64_driver_process, ev, data)
{
  static int len;
  static struct etimer e;
  PROCESS_BEGIN();

  while(1) {
    etimer_set(&e, 1);
    PROCESS_WAIT_EVENT();
    len = eth.receive();
    if(len > 0) {
      len = eth.read((char*)ip64_packet_buffer, len);
      IP64_INPUT(ip64_packet_buffer, len);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
const struct ip64_driver dp83848_ip64_driver = {
  init,
  output
};
/*---------------------------------------------------------------------------*/
