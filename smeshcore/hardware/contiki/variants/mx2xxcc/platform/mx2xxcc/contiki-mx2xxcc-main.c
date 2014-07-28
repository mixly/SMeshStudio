/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define ANNOUNCE_BOOT 1    //adds about 600 bytes to program size
#if ANNOUNCE_BOOT
#define PRINTA(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTA(...)
#endif

#define DEBUG 0
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <dev/watchdog.h>

//#include "loader/symbols-def.h"
//#include "loader/symtab.h"

#include "params.h"
#include "radio/rf230bb/rf230bb.h"
#include "net/mac/frame802154.h"
#include "net/mac/framer-802154.h"
#include "net/ipv6/sicslowpan.h"

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"

#include "dev/rs232.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#ifdef BORDER_ROUTER
PROCESS_NAME(border_router_process);
//PROCESS_NAME(webserver_nogui_process);
#endif

#if AVR_WEBSERVER
#include "httpd-fs.h"
#include "httpd-cgi.h"
#endif

#ifdef COFFEE_FILES
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"
#endif

#if UIP_CONF_ROUTER&&0
#include "net/routing/rimeroute.h"
#include "net/rime/rime-udp.h"
#endif

#include "net/rime/rime.h"

#include "dev/leds.h"
#include "dev/ds2401.h"
#ifndef UIP_CONF_EUI64
#define UIP_CONF_EUI64 1
#endif
#if UIP_CONF_EUI64
#include "params.h"
#endif

#if ARDUINO
#include "contiki-arduino.h"

unsigned char arduino_node_id[8] = { 0x02, 0x11, 0x22, 0xff, 0xfe, 0x33, 0x44, 0x33 };
unsigned char arduino_channel = MXCHANNEL;
unsigned char arduino_power = RF230_MAX_TX_POWER;
#endif

#ifdef CAMERA_INTERFACE
#include "camera.h"
#endif

#ifdef BUZZER
#include "buzzerid.h"
void
buzz_id()
{
  delayms(300);
#if UIP_CONF_EUI64
  extern uint8_t eemem_mac_address[8];
  buzzer_nodeid(eemem_mac_address[7]);
#else
  buzzer_nodeid(ds2401_id[7]);
#endif
}
#endif

static void
set_rime_addr(void)
{
	linkaddr_t addr;
#if ARDUINO
  memcpy(addr.u8, arduino_node_id, sizeof(addr.u8));
#elif UIP_CONF_EUI64
  memset(&addr, 0, sizeof(linkaddr_t));
  if (params_get_eui64(addr.u8)) {
    PRINTA("Random EUI64 address generated\n");
  }
#else
  memcpy(addr.u8, ds2401_id, sizeof(addr.u8));
#endif
#if UIP_CONF_IPV6
  memcpy(&uip_lladdr.addr, &addr.u8, sizeof(linkaddr_t));
  linkaddr_set_node_addr(&addr);
  rf230_set_pan_addr(params_get_panid(), params_get_panaddr(), (uint8_t *)&addr.u8);
#elif WITH_NODE_ID
  node_id = get_panaddr_from_eeprom();
  addr.u8[1] = node_id&0xff;
  addr.u8[0] = (node_id&0xff00) >> 8;
  PRINTA("Node ID from eeprom: %X\n", node_id);
  uint16_t inv_node_id=((node_id & 0xff00) >> 8) + ((node_id & 0xff) << 8); // change order of bytes for rf23x
  rimeaddr_set_node_addr(&addr);
  rf230_set_pan_addr(params_get_panid(), inv_node_id, NULL);
#else
  rimeaddr_set_node_addr(&addr);
  rf230_set_pan_addr(params_get_panid(), params_get_panaddr(), (uint8_t *)&addr.u8);
#endif
#if ARDUINO
  rf230_set_channel(arduino_channel);
  rf230_set_txpower(arduino_power);
#else
  rf230_set_channel(params_get_channel());
  rf230_set_txpower(params_get_txpower());
#endif
  
}


void
init_usart(void)
{
  /* First rs232 port for debugging */
  rs232_init(RS232_PORT_0, USART_BAUD_38400,
      USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  /* if you need you can use it
  rs232_init(RS232_PORT_1, USART_BAUD_38400,
      USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
*/
#if WITH_UIP || WITH_UIP6
  //slip_arch_init(USART_BAUD_38400);
  rs232_redirect_stdout(RS232_PORT_0);
#else
  rs232_redirect_stdout(RS232_PORT_0);
#endif /* WITH_UIP */
}


void
init_net(void)
{
  /* Start radio and radio receive process */

  NETSTACK_RADIO.init();

  /* Set addresses BEFORE starting tcpip process */
  set_rime_addr();

	queuebuf_init();

  NETSTACK_RDC.init();
  NETSTACK_MAC.init();

  NETSTACK_NETWORK.init();

  PRINTA("%s %s, channel %u , check rate %u Hz tx power %u\n", NETSTACK_MAC.name, NETSTACK_RDC.name, rf230_get_channel(),
      CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1 : NETSTACK_RDC.channel_check_interval()),
      rf230_get_txpower());
#if UIP_CONF_IPV6_RPL
  PRINTA("RPL Enabled\n");
#endif
#if UIP_CONF_ROUTER
  PRINTA("Routing Enabled\n");
#endif

  process_start(&tcpip_process, NULL);

#if ANNOUNCE_BOOT && UIP_CONF_IPV6
  PRINTA("Tentative link-local IPv6 address ");
  {
    uip_ds6_addr_t *lladdr;
    int i;
    lladdr = uip_ds6_get_link_local(-1);
    for(i = 0; i < 7; ++i) {
      PRINTA("%02x%02x:", lladdr->ipaddr.u8[i * 2],
          lladdr->ipaddr.u8[i * 2 + 1]);
    }
    PRINTA("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
  }

  if(!UIP_CONF_IPV6_RPL) {
    uip_ipaddr_t ipaddr;
    int i;
    uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);
    PRINTA("Tentative global IPv6 address ");
    for(i = 0; i < 7; ++i) {
      PRINTA("%02x%02x:",
          ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    PRINTA("%02x%02x\n",
        ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }
#endif /* ANNOUNCE_BOOT */


}
/* Track interrupt flow through mac, rdc and radio driver */
//#define DEBUGFLOWSIZE 32
#if DEBUGFLOWSIZE
uint8_t debugflowsize,debugflow[DEBUGFLOWSIZE];
#define DEBUGFLOW(c) if (debugflowsize<(DEBUGFLOWSIZE-1)) debugflow[debugflowsize++]=c
#else
#define DEBUGFLOW(c)
#endif

/* Get periodic prints from idle loop, from clock seconds or rtimer interrupts */
/* Use of rtimer will conflict with other rtimer interrupts such as contikimac radio cycling */
/* STAMPS will print ENERGEST outputs if that is enabled. */
#define PERIODICPRINTS 0
#if PERIODICPRINTS
//#define PINGS 64
#define ROUTES 600
#define STAMPS 60
#define STACKMONITOR 1024
uint32_t clocktime;
#define TESTRTIMER 0
#if TESTRTIMER
uint8_t rtimerflag=1;
struct rtimer rt;
void rtimercycle(void) {rtimerflag=1;}
#endif
#endif



/*-------------------------------------------------------------------------*/
/*----------------------Configuration of the .elf file---------------------*/
#if 1
/* The proper way to set the signature is */
#include <avr/signature.h>
#else
/* Older avr-gcc's may not define the needed SIGNATURE bytes. Do it manually if you get an error */
typedef struct {const unsigned char B2;const unsigned char B1;const unsigned char B0;} __signature_t;
#define SIGNATURE __signature_t __signature __attribute__((section (".signature")))
SIGNATURE = {
  .B2 = 0x01,//SIGNATURE_2, //ATMEGA128rfa1
  .B1 = 0xA7,//SIGNATURE_1, //128KB flash
  .B0 = 0x1E,//SIGNATURE_0, //Atmel
};
#endif

#if 0
/* JTAG, SPI enabled, Internal RC osc, Boot flash size 4K, 6CK+65msec delay, brownout disabled */
FUSES ={.low = 0xe2, .high = 0x99, .extended = 0xff,};
#else
/* JTAG+SPI, Boot 4096 words @ $F000, Internal  oscillator, startup 6 CK +0 ms, Brownout 1.8 volts */
FUSES ={.low = 0xC2, .high = 0x99, .extended = 0xfe,};
#endif

uint8_t
rng_get_uint8(void) {
#if 0
  /* Upper two RSSI reg bits (RND_VALUE) are random in rf231 */
  uint8_t j;
  j = (PHY_RSSI&0xc0) + ((PHY_RSSI>>2)&0x30) + ((PHY_RSSI>>4)&0x0c) + ((PHY_RSSI>>6)&0x03);
#else
/* Get a pseudo random number using the ADC */
  uint8_t i,j=0;
  ADCSRA=1<<ADEN;             //Enable ADC, not free running, interrupt disabled, fastest clock
  for (i=0;i<4;i++) {
    ADMUX = 0;                //toggle reference to increase noise
    ADMUX =0x1E;              //Select AREF as reference, measure 1.1 volt bandgap reference.
    ADCSRA|=1<<ADSC;          //Start conversion
    while (ADCSRA&(1<<ADSC)); //Wait till done
	j = (j<<2) + ADC;
  }
  ADCSRA=0;                   //Disable ADC
#endif
  PRINTD("rng issues %d\n",j);
  return j;
}


/*-------------------------Low level initialization------------------------*/
/*------Done in a subroutine to keep main routine stack usage small--------*/
static char mcuinitialized = 0;

void
initialize(void)
{
  if (mcuinitialized)
    return;
  mcuinitialized = 1;

#ifdef BUZZER
  buzz_id();
#endif

  watchdog_init();
  watchdog_start();


#ifdef CAMERA_INTERFACE
  camera_init();
#else
  init_usart();
#endif
	clock_init();



#if STACKMONITOR
  /* Simple stack pointer highwater monitor. Checks for magic numbers in the main
   * loop. In conjuction with PERIODICPRINTS, never-used stack will be printed
   * every STACKMONITOR seconds.
   */
{
extern uint16_t __bss_end;
uint16_t p=(uint16_t)&__bss_end;
    do {
      *(uint16_t *)p = 0x4242;
      p+=10;
    } while (p<SP-10); //don't overwrite our own stack
}
#endif

#define CONF_CALIBRATE_OSCCAL 0
#if CONF_CALIBRATE_OSCCAL
void calibrate_rc_osc_32k();
{
extern uint8_t osccal_calibrated;
uint8_t i;
  PRINTD("\nBefore calibration OSCCAL=%x\n",OSCCAL);
  for (i=0;i<10;i++) { 
    calibrate_rc_osc_32k();  
    PRINTD("Calibrated=%x\n",osccal_calibrated);
//#include <util/delay_basic.h>
//#define delay_us( us )   ( _delay_loop_2(1+(us*F_CPU)/4000000UL) ) 
//   delay_us(50000);
 }
   clock_init();
}
#endif 

  PRINTA("\n*******Booting %s*******\n",CONTIKI_VERSION_STRING);

  leds_init();




/* rtimers needed for radio cycling */
  rtimer_init();

 /* Initialize process subsystem */
  process_init();

  /* etimers must be started before ctimer_init */
  process_start(&etimer_process, NULL);

#if RF230BB || RF212BB
  ds2401_init();
  //node_id_restore();

  /* Get a random seed for the 802.15.4 packet sequence number.
   * Some layers will ignore duplicates found in a history (e.g. Contikimac)
   * causing the initial packets to be ignored after a short-cycle restart.
   */
  random_init(rng_get_uint8());

  ctimer_init();

  init_net();
#else /* !RF230BB || RF212BB */
/* Original RF230 combined mac/radio driver */
/* mac process must be started before tcpip process! */
  process_start(&mac_process, NULL);
  process_start(&tcpip_process, NULL);
#endif /* RF230BB || RF212BB */

#ifndef ARDUINO
  /* Autostart other processes */
  autostart_start(autostart_processes);
#endif


/*--------------------------Announce the configuration---------------------*/
#if ANNOUNCE_BOOT
	PRINTA("Online\n");
#endif /* ANNOUNCE_BOOT */

}

#if ROUTES && UIP_CONF_IPV6
static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int8_t i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) PRINTF("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        PRINTF(":");
      }
      PRINTF("%x",a);
    }
  }
}
#endif

/*-------------------------------------------------------------------------*/
/*------------------------- Main Scheduler loop----------------------------*/
/*-------------------------------------------------------------------------*/
int
main(void)
{
#if ARDUINO
  setup();
#endif

	initialize();
/*
	i2c_init();
	i2c_delay_us(100);
	real_init_i2c();
	i2c_delay_us(100);
	i2c_config(I2C_NONE);
	leds_on(LEDS_RED);
	leds_off(LEDS_RED);
*/	
	leds_on(LEDS_RED);
#ifdef BORDER_ROUTER
  process_start(&border_router_process, NULL);
  //process_start(&webserver_nogui_process, NULL);
#endif
	while(1) {
		process_run();
		watchdog_periodic();
#if UIP_CONF_IPV6
	uip_ds6_nbr_t *nbr;
#endif /* UIP_CONF_IPV6 */

#if PERIODICPRINTS
#if TESTRTIMER
		/* Timeout can be increased up to 8 seconds maximum.
		 * A one second cycle is convenient for triggering the various debug printouts.
		 * The triggers are staggered to avoid printing everything at once.
		 */
		if (rtimerflag) {
			rtimer_set(&rt, RTIMER_NOW()+ RTIMER_ARCH_SECOND*1UL, 1,(void *) rtimercycle, NULL);
			rtimerflag=0;
#else
  if (clocktime!=clock_seconds()) {
     clocktime=clock_seconds();
#endif

#if STAMPS
if ((clocktime%STAMPS)==0) {
#if ENERGEST_CONF_ON
#include "lib/print-stats.h"
  print_stats();
#elif RADIOSTATS
extern volatile unsigned long radioontime;
  PRINTF("%u(%u)s\n",clocktime,radioontime);
#else
  PRINTF("%us\n",clocktime);
#endif

}
#endif
#if TESTRTIMER
      clocktime+=1;
#endif

#if PINGS && UIP_CONF_IPV6
extern void raven_ping6(void); 
if ((clocktime%PINGS)==1) {
  PRINTF("**Ping\n");
  raven_ping6();
}
#endif

#if ROUTES && UIP_CONF_IPV6
if ((clocktime%ROUTES)==2) {
      
extern uip_ds6_netif_t uip_ds6_if;

  uint8_t i,j=0;
  PRINTF("\nAddresses [%u max]\n",UIP_DS6_ADDR_NB);
  for (i=0;i<UIP_DS6_ADDR_NB;i++) {
    if (uip_ds6_if.addr_list[i].isused) {
      ipaddr_add(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
  PRINTF("\nNeighbors [%u max]\n",NBR_TABLE_MAX_NEIGHBORS);

  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {
    ipaddr_add(&nbr->ipaddr);
    PRINTF("\n");
    j=0;
  }
  if (j) PRINTF("  <none>");
  PRINTF("\nRoutes [%u max]\n",UIP_DS6_ROUTE_NB);
  {
    uip_ds6_route_t *r;
    PRINTF("\nRoutes [%u max]\n",UIP_DS6_ROUTE_NB);
    j = 1;
    for(r = uip_ds6_route_head();
        r != NULL;
        r = uip_ds6_route_next(r)) {
      ipaddr_add(&r->ipaddr);
      PRINTF("/%u (via ", r->length);
      ipaddr_add(uip_ds6_route_nexthop(r));
      PRINTF(") %lus\n", r->state.lifetime);
      j = 0;
    }
  }
  if (j) PRINTF("  <none>");
  PRINTF("\n---------\n");
}
#endif

#if STACKMONITOR
if ((clocktime%STACKMONITOR)==3) {
  extern uint16_t __bss_end;
  uint16_t p=(uint16_t)&__bss_end;
  do {
    if (*(uint16_t *)p != 0x4242) {
      PRINTF("Never-used stack > %d bytes\n",p-(uint16_t)&__bss_end);
      break;
    }
    p+=10;
  } while (p<RAMEND-10);
}
#endif

    }
#endif /* PERIODICPRINTS */

  }
  return 0;
}

/*---------------------------------------------------------------------------*/

void log_message(char *m1, char *m2)
{
  PRINTF("%s%s\n", m1, m2);
}
