/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera.c $
 */

/**
 * \file
 *         Camera operation via a serial connection.
 * \author
 *         SmeshLink
 */

#include "contiki.h"

#include "dev/rs232.h"
#include "sys/energest.h"
#include "camera.h"

#ifndef CAMERA_PORT
#define CAMERA_PORT RS232_PORT_0
#endif

#define CAMERA_CMD_H 0x48
#define CAMERA_CMD_R 0x52
#define CAMERA_CMD_E 0x45
#define CAMERA_CMD_F 0x46

#define SOF_CHAR 0x55
#define EOF_CHAR 0x23

#define MAX_DELAY 1000000
#define CMD_LEN 128

static struct {
  uint8_t cmd;
  union {
    uint32_t size;
    struct {
      uint16_t no;
      uint16_t len;
    };
  };
  uint8_t frame[CMD_LEN];
  uint16_t checksum;
  uint16_t count;
  uint16_t ndx;
  uint8_t done;
} cmd;

/*
 * Send command through rs232
 */
static void
send_frame(uint8_t cmd_char, uint8_t len, uint8_t *payload)
{
  uint8_t i = 0;
//#if LOWPOWER
 // setcansleepstate(0);
//#endif
  cmd.done = 0;
  rs232_send(CAMERA_PORT, SOF_CHAR);
  rs232_send(CAMERA_PORT, cmd_char);
  for (; i < len; i++) {
    rs232_send(CAMERA_PORT, *payload++);
  }
  cmd.ndx = 0;
  rs232_send(CAMERA_PORT, EOF_CHAR);
}

/*
 * Parse the size of picture.
 */
camera_size_t
camera_parse_size(char size)
{
  if (CAMERA_SIZE_2 == size) {
    return CAMERA_SIZE_2;
  } else if (CAMERA_SIZE_3 == size) {
    return CAMERA_SIZE_3;
  } else {
    return CAMERA_SIZE_1;
  }
}

/*
 * Take a picture.
 */
void
camera_take_picture(camera_size_t camera_size, uint16_t packet_size, uint32_t *size, uint16_t *count)
{
  uint32_t counter = MAX_DELAY;
  uint8_t payload[3];
  payload[0] = camera_size;
  payload[1] = packet_size & 0xFF;
  payload[2] = (packet_size >> 8) & 0xFF;
  cmd.count = 0;
  send_frame(CAMERA_CMD_H, 3, payload);
  while ((!cmd.done || CAMERA_CMD_R != cmd.cmd) && counter--) {
    /* Waiting for response. */
    /* A trick for enabling current loop to be interrupted. */
    *count = 0;
  }
  *size = camera_get_picture_size();
  *count = camera_get_packet_count();
//#if LOWPOWER
 // setcansleepstate(1);
//#endif
}

/*
 * Get the byte size of the picture.
 */
uint32_t
camera_get_picture_size()
{
  if (CAMERA_CMD_R == cmd.cmd && cmd.done) {
    return cmd.size;
  } else {
    return 0;
  }
}

/*
 * Get the total count of the picture's packets.
 */
uint16_t
camera_get_packet_count()
{
  if (CAMERA_CMD_R == cmd.cmd && cmd.done && !cmd.count) {
    cmd.count = cmd.frame[0] + ((cmd.frame[1] & 0xFF) << 8);
  }
  return cmd.count;
}

/*
 * Get one packet specified by packet_no.
 */
uint16_t
camera_get_packet(uint16_t packet_no, uint8_t *buffer)
{
  uint8_t payload[2];
  uint16_t i = 0, count = camera_get_packet_count();
  uint32_t counter = MAX_DELAY;

  if (0 == packet_no || packet_no > count) {
    return 0;
  }

  payload[0] = packet_no;
  payload[1] = packet_no >> 8;
  send_frame(CAMERA_CMD_E, 2, payload);
  while ((!cmd.done || CAMERA_CMD_F != cmd.cmd) && counter--) {
    /* Waiting for response. */
    /* A trick for enabling current loop to be interrupted. */
    *buffer = 0;
  }
  if (!cmd.done) {
    return 0;
  }
  for (; i < cmd.len; i++) {
    buffer[i] = cmd.frame[i];
  }
//#if LOWPOWER
 // setcansleepstate(1);
//#endif
  return cmd.len;
}

/*
 * Try to get one packet in certain times of tries.
 */
uint16_t
camera_try_get_packet(uint16_t packet_no, uint8_t *buffer, uint8_t tries)
{
  uint16_t len = 0;
  while (tries--) {
    len = camera_get_packet(packet_no, buffer);
    if (len > 0)
      return len;
  }
  return 0;
}

#define DEBUG_CAMERA 1
#if DEBUG_CAMERA

#include <stdio.h>
#include <string.h>

int
camera_debug(char* buf)
{
  int index = 0;
  index += sprintf(buf + index, "cmd:%x,", cmd.cmd);
  index += sprintf(buf + index, "done:%d,", cmd.done);
  index += sprintf(buf + index, "ndx:%d,", cmd.ndx);
  index += sprintf(buf + index, "size:%lu,", cmd.size);
  index += sprintf(buf + index, "no:%u,", cmd.no);
  index += sprintf(buf + index, "len:%u,", cmd.len);
  index += sprintf(buf + index, "checksum:%u,", cmd.checksum);
  index += sprintf(buf + index, "count:%u,", cmd.count);
  //for (i = 0; i < 10; i++) {
  //  index += sprintf(temp + index, " %02x", cmd.frame[i]);
  //}
  return index;
}

#endif

/*
 * Process an input character from serial port.
 * ** This is called from an ISR!!
 */
int
camera_serial_input(unsigned char ch)
{
  switch (cmd.ndx) {
  case 0:
    /* 1st byte, must be 0x55/'U'. */
    cmd.done = 0;
    if (SOF_CHAR != ch) {
        return 0;
    }
    break;
  case 1:
    /* 2nd byte, command byte. */
    cmd.cmd = ch;
    break;
  case 2:
    /* 3rd byte, 0x23/'#' (response of H/E) or 1st byte of size. */
    if ((CAMERA_CMD_H == cmd.cmd || CAMERA_CMD_E == cmd.cmd) && EOF_CHAR == ch) {
      /* End of command */
      cmd.done = 1;
      cmd.ndx = 0;
      return 0;
    } else {
      /* 1st byte of number */
      cmd.size = (uint32_t)ch & 0x000000FF;
    }
    break;
  case 3:
    /* 4th byte, 2nd byte of size. */
    cmd.size += ((uint32_t)ch & 0x000000FF) << 8;
    break;
  case 4:
    /* 5th byte, 3rd byte of size. */
    cmd.size += ((uint32_t)ch & 0x000000FF) << 16;
    break;
  case 5:
    /* 6th byte, 4th byte of size. */
    cmd.size += ((uint32_t)ch & 0x000000FF) << 24;
    break;
  case 6:
  case 7:
    cmd.frame[cmd.ndx - 6] = ch;
    break;
  case 8:
    /* 9th byte, might be '#' (response of R). */
    if (CAMERA_CMD_R == cmd.cmd && EOF_CHAR == ch) {
      cmd.done = 1;
      cmd.ndx = 0;
      return 0;
    } else {
      cmd.frame[cmd.ndx - 6] = ch;
    }
    break;
  default:
    /* Payload and checksum. */
    if (cmd.ndx >= cmd.len + 6) {
      /* All done, check checksum. */
      cmd.done++;
      cmd.frame[cmd.ndx - 6] = ch;
      if (cmd.ndx >= cmd.len + 7) {
        /* 2nd byte of checksum */
        cmd.checksum += ((uint16_t)ch & 0x00FF) << 8;
        /* Check the checksum */
        uint16_t sum = 0, ii = 0;
        for (; ii < cmd.len; ii++) {
          sum += cmd.frame[ii];
        }
        if (sum == cmd.checksum) {
          cmd.done = 1;
        }
        cmd.ndx = 0;
        return 0;
      } else {
        /* 1st byte of checksum. */
        cmd.checksum = ch;
      }
    } else {
      /* Store data. */
      cmd.frame[cmd.ndx - 6] = ch;
    }
    break;
  }

  cmd.ndx++;
  return 0;
}

void
camera_init()
{
  //setcansleepstate(0);
  rs232_init(CAMERA_PORT, USART_BAUD_38400, USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  rs232_set_input(CAMERA_PORT, camera_serial_input);
}
