#include "Queue.h"
QUEUE::QUEUE() {
	front=rear=0;
}
void QUEUE::init_queue()
{
	front=rear=0;
}
uint8_t QUEUE::peerqueue()
{
	uint8_t t=RFQUENEMAX;
	if(front==rear)//队列是空
	{
		return RFQUENEMAX;
	}
	t=front;
	return t;
}
uint8_t QUEUE::inqueue()
{
	uint8_t t=RFQUENEMAX;
	if((rear+1)%RFQUENEMAX==front)//队列满
	{
		return RFQUENEMAX ;
	}
	else//非满
	{
		t=rear;
		rear=(rear+1)%RFQUENEMAX;
	}
	return t;
}
uint8_t QUEUE::dequeue()
{
	uint8_t t=RFQUENEMAX;
	if(front==rear)//队列是空
	{
		return RFQUENEMAX;
	}
	t=front;
	front=(front+1) % RFQUENEMAX;
	return t;
}
void QUEUE::undodequeue() //used for no ack get
{
	front=(front+RFQUENEMAX-1) % RFQUENEMAX;
	return ;
}
