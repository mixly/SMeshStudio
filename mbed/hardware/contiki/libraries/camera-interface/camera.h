/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera.h $
 */

/**
 * \file
 *         Camera interface.
 * \author
 *         SmeshLink
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#define DEFAULT_CAMERA_PACKET_SIZE 20

typedef enum camera_size_t
{
  CAMERA_SIZE_1 = 0x31,
  CAMERA_SIZE_2,
  CAMERA_SIZE_3,
} camera_size_t;

void camera_init();

/*
 * Parse the size of picture.
 */
camera_size_t camera_parse_size(char size);
/*
 * Take a picture.
 */
void camera_take_picture(camera_size_t camera_size, uint16_t packet_size, uint32_t *size, uint16_t *count);
/*
 * Get the byte size of the picture.
 */
uint32_t camera_get_picture_size();
/*
 * Get the total count of the picture's packets.
 */
uint16_t camera_get_packet_count();
/*
 * Get one packet specified by packet_no.
 */
uint16_t camera_get_packet(uint16_t packet_no, uint8_t *buffer);
/*
 * Try to get one packet in certain times of tries.
 */
uint16_t camera_try_get_packet(uint16_t packet_no, uint8_t *buffer, uint8_t tries);

#endif /* __CAMERA_H__ */
