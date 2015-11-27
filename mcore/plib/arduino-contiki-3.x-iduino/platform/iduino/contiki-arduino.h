/*
 * Copyright (c) 2012-2013, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 */

#ifndef __CONTIKI_ARDUINO_H__
#define __CONTIKI_ARDUINO_H__


extern unsigned char arduino_node_id[8];
extern unsigned char arduino_channel;
extern unsigned char arduino_power;

#define arduino_set_node_id(id0, id1, id2, id3, id4, id5, id6, id7) do { \
	arduino_node_id[0] = id0; \
	arduino_node_id[1] = id1; \
	arduino_node_id[2] = id2; \
	arduino_node_id[3] = id3; \
	arduino_node_id[4] = id4; \
	arduino_node_id[5] = id5; \
	arduino_node_id[6] = id6; \
	arduino_node_id[7] = id7; \
	} while (0)
#define arduino_set_channel(channel) arduino_channel = channel
#define arduino_set_power(power) arduino_power = power

void setup();
void initialize();



#endif /* __CONTIKI_ARDUINO_H__ */
