/*
 *  QUEUE.h
 *
 *  Created on: 2013-5-9
 *      Author: fu
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "stdint.h"
#define PACKAGE_MAX 128
#define RFQUENEMAX 100


struct RfData
{
	uint8_t rbuf[PACKAGE_MAX];
	uint8_t length;
	uint8_t payloadindex;
	union
	{
		uint8_t rssi;
		uint8_t destaddress;
	} value;
};

class  QUEUE {
private:

	uint8_t front;
	uint8_t rear;;
public:
	struct RfData RfData[RFQUENEMAX];
	QUEUE();
	void init_queue();
	uint8_t inqueue( );
	uint8_t dequeue();
	void undodequeue();
	uint8_t peerqueue();

};

#endif /* QUEUE_H_ */
