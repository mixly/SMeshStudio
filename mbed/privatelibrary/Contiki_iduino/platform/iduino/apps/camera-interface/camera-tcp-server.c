/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera-tcp-server.c $
 */

/**
 * \file
 *         TCP server for camera application.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"

#include "camera.h"

#define CAMERA_PORT 9000

static struct psock ps;
static uint8_t buffer[128];

/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_connection(struct psock *p))
{
  static uint8_t tries = 10;
  static camera_size_t camera_size;
  static uint16_t index, count, len;
  static uint32_t size;

  PSOCK_BEGIN(p);

  PSOCK_WAIT_UNTIL(p, PSOCK_NEWDATA(p));

  if (PSOCK_NEWDATA(p)) {
    /* Read the picture size. */
    PSOCK_READTO(p, '\n');

    camera_size = camera_parse_size(buffer[0]);
    camera_take_picture(camera_size, DEFAULT_CAMERA_PACKET_SIZE, &size, &count);

    /* Send picture size and packet count. */
    buffer[0] = size;
    buffer[1] = size >> 8;
    buffer[2] = size >> 16;
    buffer[3] = size >> 24;
    buffer[4] = count;
    buffer[5] = count >> 8;
    PSOCK_SEND(p, buffer, 6);

    /* Send picture packet by packet. */
    for (index = 1; index <= count; index++) {
      len = camera_try_get_packet(index, buffer, tries);
      PSOCK_SEND(p, buffer, len);
    }
  } else {
    /* Time is out. */
  }

  PSOCK_CLOSE(p);

  PSOCK_END(p);
}
/*---------------------------------------------------------------------------*/
PROCESS(camera_tcp_server, "Camera TCP server");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(camera_tcp_server, ev, data)
{
  PROCESS_BEGIN();

  tcp_listen(uip_htons(CAMERA_PORT));

  /* Loop forever, accepting new connections. */
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);

    if (uip_connected()) {
      PSOCK_INIT(&ps, buffer, sizeof(buffer));
      while(!(uip_aborted() || uip_closed() || uip_timedout())) {
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
        handle_connection(&ps);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
