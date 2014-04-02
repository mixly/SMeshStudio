#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <sys/etimer.h>
#include <sys/autostart.h>
#include <sys/clock.h>
#include "contiki-net.h"
//#include <net/dhcpc.h>
//#include "emac-driver.h"
//#include "hal_radio.h"

#include "contiki-conf.h"
#include "rf230bb.h"
#include <net/uip-debug.h>
#include "net/mac/frame802154.h"
#include "net/mac/framer-802154.h"
extern void rf230_set_channel(uint8_t c);

#ifdef __cplusplus
extern "C" {
#endif
#include "ethernet_api.h"
#ifdef __cplusplus
}
#endif

void mbed_mac_address(char *s)
{
  uint8_t macaddr[6];

  macaddr[0] = TEST_MAC_ADDR0;
  macaddr[1] = TEST_MAC_ADDR1;
  macaddr[2] = TEST_MAC_ADDR2;
  macaddr[3] = TEST_MAC_ADDR3;
  macaddr[4] = TEST_MAC_ADDR4;
  macaddr[5] = TEST_MAC_ADDR5;
  macaddr[6] = TEST_MAC_ADDR6;

  memcpy(s,macaddr,sizeof(macaddr));
  printf("MAC addr %02x:%02x:%02x:%02x:%02x:%02x\n",
         *s, *(s+1), *(s+2), *(s+3), *(s+4), *(s+5));
}

int
main()
{
//  debug_uart_setup();
  printf("********************* ");
  printf("Welcome to Xbed");
  printf(" ********************\n");
  printf("Initializing clocks\n");
  clock_init();
  ctimer_init();
  rtimer_init();
//  printf("The system main CPU clock speed is %dHz\n",SystemCoreClock);
  printf("Initializing processes\n");
  process_init();
//
  printf("Starting etimers\n");
  process_start(&etimer_process, NULL);
////  printf("Starting EMAC service\n");
////  process_start(&emac_lpc1768, NULL);
////
  // init MAC address
//  uip_lladdr.addr[0] = TEST_MAC_ADDR0;
//  uip_lladdr.addr[1] = TEST_MAC_ADDR1;
//  uip_lladdr.addr[2] = TEST_MAC_ADDR2;
//  uip_lladdr.addr[3] = TEST_MAC_ADDR3;
//  uip_lladdr.addr[4] = TEST_MAC_ADDR4;
//  uip_lladdr.addr[5] = TEST_MAC_ADDR5;
//  uip_lladdr.addr[6] = TEST_MAC_ADDR6;
//
//   Configure global IPv6 address
//  uip_ipaddr_t ipaddr;
//  uip_ip6addr(&ipaddr, 0x2000, 0, 0, 0, 0, 0, 0, 0);
//  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
//  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
//  printf("Manual global IPv6 address: ");
//  uip_debug_ipaddr_print(&ipaddr);
//  printf("\n");
//
  rimeaddr_t addr;
  ethernet_address(&addr);
  memcpy(&uip_lladdr.addr, &addr.u8, 8);
//  memcpy(&addr.u8, &uip_lladdr.addr, 8);

//
//  // Add prefix of manually set global address to prefix list
////  uip_ds6_prefix_add(&ipaddr, 64, 0);
  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();
  rimeaddr_set_node_addr(&addr);
//  rf230_set_channel(CHANNEL_802_15_4);
  rf230_set_pan_addr(IEEE802154_PANID,0,(uint8_t *)&addr.u8);
  printf("%s %s, channel check rate %lu Hz, radio channel %u\n",
         NETSTACK_MAC.name, NETSTACK_RDC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:
                         NETSTACK_RDC.channel_check_interval()),
                         CHANNEL_802_15_4);
  printf("Starting TCP/IP service\n");
  process_start(&tcpip_process, NULL);          // invokes uip_init();
  {
	  uip_ds6_addr_t *lladdr;
  	  lladdr = uip_ds6_get_link_local(-1);

	  printf("Tentative link-local IPv6 address:");
	  uip_debug_ipaddr_print(&(lladdr->ipaddr));
	  printf("\n");
  }

//  printf("Starting the Bang-Bang control process\n");
//  process_start(&bang_control_process, NULL);
  //test began
//  /* Initialize process subsystem */
//  process_init();
//
//  /* etimers must be started before ctimer_init */
//  process_start(&etimer_process, NULL);
//
//  /* Start radio and radio receive process */
//  NETSTACK_RADIO.init();
//
//  /* Set addresses BEFORE starting tcpip process */
//
//  rimeaddr_t addr;
////  memset(&addr, 0, sizeof(rimeaddr_t));
////  get_mac_from_eeprom(addr.u8);
//  addr.u8[0] = TEST_MAC_ADDR0;
//  addr.u8[1] = TEST_MAC_ADDR1;
//  addr.u8[2] = TEST_MAC_ADDR2;
//  addr.u8[3] = TEST_MAC_ADDR3;
//  addr.u8[4] = TEST_MAC_ADDR4;
//  addr.u8[5] = TEST_MAC_ADDR5;
//  addr.u8[6] = TEST_MAC_ADDR6;
//  addr.u8[7] = TEST_MAC_ADDR7;
//
//  memcpy(&uip_lladdr.addr, &addr.u8, 8);
//
//// Is this required with IPv6, I wonder?
//#if 1
//  rf230_set_pan_addr(
//    IEEE802154_PANID,
//    0,
//    (uint8_t *)&addr.u8
//  );
//  rf230_set_channel(CHANNEL_802_15_4);
//
//  rimeaddr_set_node_addr(&addr);
//#endif
//
//  PRINTF("MAC address %x:%x:%x:%x:%x:%x:%x:%x\n",addr.u8[0],addr.u8[1],addr.u8[2],addr.u8[3],addr.u8[4],addr.u8[5],addr.u8[6],addr.u8[7]);
//
//  /* Initialize stack protocols */
//  queuebuf_init();
//  /* Check is it sniffer */
//  NETSTACK_RDC.init();
//  NETSTACK_MAC.init();
//  NETSTACK_NETWORK.init();
//
//  process_start(&tcpip_process, NULL);
//
//  /* Add easy addresses for testing */
//  uip_ip6addr_t ipaddr;
//  uip_ip6addr(&ipaddr, 0xfdfd, 0, 0, 0, 0, 0, 0, addr.u8[7]);
//  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
//#if UIP_CONF_ROUTER
//  uip_ds6_prefix_add(&ipaddr,64,0,0,0,0);
//#else
//  uip_ds6_prefix_add(&ipaddr,64,0);
//#endif
//
//  /* Create a route through the border router for site-local addresses, fec0::/64. */
//  uip_ds6_route_t *rep;
//  uip_ip6addr_t next_hop;
//  uip_ip6addr(&ipaddr, 0xfec0, 0, 0, 0, 0, 0, 0, 0);
//  uip_ip6addr(&next_hop, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
//  if((rep = uip_ds6_route_add(&ipaddr, 64, &next_hop)) == NULL)
//  {
//    printf("*** Failed to add a route to fec0::/64\n");
//  }
  //test end
  autostart_start(autostart_processes);

  printf("Processes running\n");
  while (1){

      do
        {
        }
      while (process_run() > 0);
//      idle_count++;
      /* Idle! */
      /* Stop processor clock */
      /* asm("wfi"::); */
    }
  return 0;
}
