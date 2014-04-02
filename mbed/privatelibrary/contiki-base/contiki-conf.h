#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

#include <stdint.h>
//#include "platform-conf.h"
//#include "lpc1768.h"

///* For user configure */
//#define IP64_ENABLE 0
//#define SLIP_ENABLE 1
#define SLIP_ENABLE    0

/* For user configure end*/

#define CCIF
#define CLIF

#define WITH_UIP 1
#define WITH_ASCII 1

#define CLOCK_CONF_SECOND 1000

//UART used for sending printf messages
//Defined as 0 for UART0 and 1 for UART1
//#define DEBUG_UART      0

//#include "ctk/ctk-vncarch.h"
//
//#define LOG_CONF_ENABLED        0
#define AUTOSTART_ENABLE        1
#define UIP_CONF_IPV6           1
typedef unsigned long rtimer_clock_t;
#define RTIMER_CLOCK_LT(a,b)     ((signed long)((a)-(b)) < 0)
/* Define the MAC address of the device */
/* 0C 1D 12 E0 1F 10*/
#define EMAC_ADDR0              0x10
#define EMAC_ADDR1              0x1F
#define EMAC_ADDR2              0xE0
#define EMAC_ADDR3              0x12
#define EMAC_ADDR4              0x1D
#define EMAC_ADDR5              0x0C

#define TEST_MAC_ADDR0              0x00
#define TEST_MAC_ADDR1              0x1A
#define TEST_MAC_ADDR2              0x4C
#define TEST_MAC_ADDR3              0x01
#define TEST_MAC_ADDR4              0x15
#define TEST_MAC_ADDR5              0x1A
#define TEST_MAC_ADDR6              0xDC
#define TEST_MAC_ADDR7              0xB1

/* These names are deprecated, use C99 names. */
//typedef uint8_t u8_t;
//typedef uint16_t u16_t;
//typedef uint32_t u32_t;
//typedef int8_t s8_t;
//typedef int16_t s16_t;
//typedef int32_t s32_t;
//typedef unsigned long off_t;

typedef uint32_t clock_time_t;
typedef unsigned int uip_stats_t;

#ifndef BV
#define BV(x) (1<<(x))
#endif

/* uIP configuration */
//Ethernet LLH(Link Level Header) size is 14 bytes

/* MIST_ENABLE end */

#define UIP_CONF_BROADCAST       1
//#define UIP_CONF_LOGGING 1
//#define UIP_CONF_BUFFER_SIZE 1024
#define UIP_CONF_TCP_FORWARD 1
#define UIP_CONF_ICMP6 1

/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

#define USB_EP1_SIZE 64
#define USB_EP2_SIZE 64

//#define RAND_MAX 0x7fff

//uip.h's method will make cpu dead, so we use followed method.
#define uip_ipaddr_copy(dest, src)		\
    memcpy(dest, src, sizeof(*dest))
#define uip_ip4addr_copy(dest, src)		\
    memcpy(dest, src, sizeof(*dest))
#define uip_ip6addr_copy(dest, src)		\
    memcpy(dest, src, sizeof(*dest))
/* ************************************************************************** */
//#pragma mark RPL Settings
/* ************************************************************************** */

/* Define MAX_*X_POWER to reduce tx power and ignore weak rx packets for testing a miniature multihop network.
 * Leave undefined for full power and sensitivity.
 * tx=0 (3dbm, default) to 15 (-17.2dbm)
 * RF230_CONF_AUTOACK sets the extended mode using the energy-detect register with rx=0 (-91dBm) to 84 (-7dBm)
 *   else the rssi register is used having range 0 (91dBm) to 28 (-10dBm)
 *   For simplicity RF230_MIN_RX_POWER is based on the energy-detect value and divided by 3 when autoack is not set.
 * On the RF230 a reduced rx power threshold will not prevent autoack if enabled and requested.
 * These numbers applied to both Raven and Jackdaw give a maximum communication distance of about 15 cm
 * and a 10 meter range to a full-sensitivity RF230 sniffer.
#define RF230_MAX_TX_POWER 15
#define RF230_MIN_RX_POWER 30
 */

#define UIP_CONF_ROUTER                 1
#define UIP_CONF_ND6_SEND_RA		    0
#define UIP_CONF_ND6_REACHABLE_TIME     600000
#define UIP_CONF_ND6_RETRANS_TIMER      10000

#undef UIP_CONF_UDP_CONNS
#define UIP_CONF_UDP_CONNS       12
#undef UIP_CONF_FWCACHE_SIZE
#define UIP_CONF_FWCACHE_SIZE    30
#define UIP_CONF_BROADCAST       1
#define UIP_ARCH_IPCHKSUM        1
#define UIP_CONF_PINGADDRCONF    0
#define UIP_CONF_LOGGING         0

#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL         1
#endif

/* See uip-ds6.h */
//ckt: important
#define NBR_TABLE_CONF_MAX_NEIGHBORS      25
#define UIP_CONF_MAX_ROUTES    25
//ckt: end
#define UIP_CONF_DS6_NBR_NBU      20
#define UIP_CONF_DS6_DEFRT_NBU    2
#define UIP_CONF_DS6_PREFIX_NBU   3
#define UIP_CONF_DS6_ROUTE_NBU    20
#define UIP_CONF_DS6_ADDR_NBU     3
#define UIP_CONF_DS6_MADDR_NBU    0
#define UIP_CONF_DS6_AADDR_NBU    0

#define UIP_CONF_LL_802154        1

/* 10 bytes per stateful address context - see sicslowpan.c */
/* Default is 1 context with prefix aaaa::/64 */
/* These must agree with all the other nodes or there will be a failure to communicate! */
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS 1
#define SICSLOWPAN_CONF_ADDR_CONTEXT_0 {addr_contexts[0].prefix[0]=0xaa;addr_contexts[0].prefix[1]=0xaa;}
#define SICSLOWPAN_CONF_ADDR_CONTEXT_1 {addr_contexts[1].prefix[0]=0xbb;addr_contexts[1].prefix[1]=0xbb;}
#define SICSLOWPAN_CONF_ADDR_CONTEXT_2 {addr_contexts[2].prefix[0]=0x20;addr_contexts[2].prefix[1]=0x01;addr_contexts[2].prefix[2]=0x49;addr_contexts[2].prefix[3]=0x78,addr_contexts[2].prefix[4]=0x1d;addr_contexts[2].prefix[5]=0xb1;}

/* 211 bytes per queue buffer */
#define QUEUEBUF_CONF_NUM         8

/* 54 bytes per queue ref buffer */
#define QUEUEBUF_CONF_REF_NUM     2

/* Take the default TCP maximum segment size for efficiency and simpler wireshark captures */
/* Use this to prevent 6LowPAN fragmentation (whether or not fragmentation is enabled) */
//#define UIP_CONF_TCP_MSS       48

/* 30 bytes per TCP connection */
/* 6LoWPAN does not do well with concurrent TCP streams, as new browser GETs collide with packets coming */
/* from previous GETs, causing decreased throughput, retransmissions, and timeouts. Increase to study this. */
/* ACKs to other ports become interleaved with computation-intensive GETs, so ACKs are particularly missed. */
/* Increasing the number of packet receive buffers in RAM helps to keep ACKs from being lost */

#define UIP_CONF_MAX_CONNECTIONS 4

/* 2 bytes per TCP listening port */
#define UIP_CONF_MAX_LISTENPORTS 4

/* 25 bytes per UDP connection */
//#define UIP_CONF_UDP_CONNS      10

#define UIP_CONF_IP_FORWARD      0
//#define UIP_CONF_FWCACHE_SIZE    0

#define UIP_CONF_IPV6_CHECKS     1
#define UIP_CONF_IPV6_QUEUE_PKT  1
#define UIP_CONF_IPV6_REASSEMBLY 0

#define UIP_CONF_UDP_CHECKSUMS   1
#define UIP_CONF_TCP_SPLIT       1
#define UIP_CONF_DHCP_LIGHT      1

/* UIP_CONF_ROUTER specifies if the IPv6 node should be a router or
   not. By default, all Contiki nodes are routers. */
#ifndef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER 0
#endif /* UIP_CONF_ROUTER */

#define RIMEADDR_CONF_SIZE        8
#define UIP_CONF_UDP              1
#define UIP_CONF_TCP              1
#define NETSTACK_CONF_NETWORK     sicslowpan_driver
#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_HC06

//#define NETSTACK_CONF_MAC         nullmac_driver
#define NETSTACK_CONF_MAC         csma_driver
#define NETSTACK_CONF_RDC         nullrdc_driver////contikimac_driver//
#define NETSTACK_CONF_FRAMER      framer_802154
#define NETSTACK_CONF_RADIO       rf230_driver
#define CHANNEL_802_15_4          11
#define RTIMER_CONF_NESTED_INTERRUPTS 1
#define CONTIKIMAC_CONF_SHORTEST_PACKET_SIZE 43-18
#define CONTIKIMAC_CONF_AFTER_ACK_DETECTECT_WAIT_TIME RTIMER_ARCH_SECOND / 1400
#define RDC_CONF_HARDWARE_ACK	  1
#define NULLRDC_802154_AUTOACK_HW 1
#define RF230_CONF_AUTOACK        1
#define RDC_CONF_HARDWARE_CSMA	  1
#define RF230_CONF_AUTORETRIES    5
#define SICSLOWPAN_CONF_FRAG      1
#define SICSLOWPAN_CONF_MAXAGE    3
#define CONTIKIMAC_CONF_WITH_PHASE_OPTIMIZATION 0
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 16
#define CONTIKIMAC_CONF_WITH_CONTIKIMAC_HEADER 0
#ifndef RPL_CONF_LEAF_ONLY
#define RPL_CONF_LEAF_ONLY		0
#endif
#define CONTIKIMAC_CONF_COMPOWER               1
#define RIMESTATS_CONF_ENABLED                 1
//ckt test
#define SICSLOWPAN_CONF_MAX_MAC_TRANSMISSIONS  4
#define PROCESS_CONF_NO_PROCESS_NAMES          1

/* MIST_ENABLE || SLIP_ENABLE */
#if MIST_ENABLE == 1
//#include "mist-conf.h"
#define UIP_CONF_LLH_LEN 14

#elif SLIP_ENABLE == 1
//#include "slip-conf.h"
#define UIP_CONF_LLH_LEN 0

#define SLIP_ENABLE    1

#define UIP_FALLBACK_INTERFACE rpl_interface
#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          4

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    140

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60

#undef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2

#else
#define UIP_CONF_LLH_LEN 0

#endif/* MIST_ENABLE || SLIP_ENABLE */

#endif /* __CONTIKI_CONF_H__ */
