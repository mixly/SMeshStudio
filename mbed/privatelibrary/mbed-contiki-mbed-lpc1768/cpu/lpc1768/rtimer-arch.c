
#include "sys/rtimer.h"
#include "rtimer-arch.h"

#define OCIE3C	OCIE3B
#define OCF3C	OCF3B

void
rtimer_arch_init(void)
{

}
/*---------------------------------------------------------------------------*/
void
rtimer_arch_schedule(rtimer_clock_t t)
{
	printf("rtimer_arch_schedule is blank\n");
}


void
rtimer_arch_sleep(rtimer_clock_t howlong)
{

}

//rtimer_clock_t rtimer_arch_now(void)
//{
//
//}
