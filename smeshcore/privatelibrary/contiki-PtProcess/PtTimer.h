/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * PtTimer.h
 *
 *      Timer based on PtProcess.
 *
 *      Author: Long
 */

#ifndef _PT_TIMER_H_
#define _PT_TIMER_H_

#include "PtProcess.h"

/*
 * Timer based on PtProcess and Contiki etimer.
 */
class PtTimer : public PtProcess {
private:
	/*
	 * the interval to timeout
	 */
	clock_time_t interval;
	/*
	 * the delay before starting the timer
	 */
	clock_time_t delay;
	/*
	 * the associated etimer
	 */
	struct etimer timer;
	/*
	 * the target PT_THREAD function to run
	 */
	PT_THREAD((*target)());
public:
	/*
	 * Instantiates a timer.
	 *
	 * \param interval the interval to timeout
	 * \param delay (optional) the delay before starting timing
	 * \param name (optional) the name of this timer
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
//	PtTimer(clock_time_t interval, clock_time_t delay = 0, const char *name = NULL, bool autostart = true);
	/*
	 * Instantiates a timer.
	 *
	 * \param interval the interval to timeout in seconds
	 * \param delay (optional) the delay before starting timing in seconds
	 * \param name (optional) the name of this timer
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
	PtTimer(float interval, float delay = 0, const char *name = NULL, bool autostart = true);
	/*
	 * Instantiates a timer.
	 *
	 * \param target the target function to run periodically
	 * \param interval the interval to timeout
	 * \param delay (optional) the delay before starting timing
	 * \param name (optional) the name of this timer
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
//	PtTimer(PT_THREAD((*target)()), clock_time_t interval, clock_time_t delay = 0, const char *name = NULL, bool autostart = true);
	/*
	 * Instantiates a timer.
	 *
	 * \param target the target function to run periodically
	 * \param interval the interval to timeout in seconds
	 * \param delay (optional) the delay before starting timing in seconds
	 * \param name (optional) the name of this timer
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
	PtTimer(PT_THREAD((*target)()), float interval, float delay = 0, const char *name = NULL, bool autostart = true);
#if !CC_NO_VIRTUAL_DESTRUCTOR
	virtual
#endif
	~PtTimer();
protected:
	PT_THREAD(run(struct pt *process_pt, process_event_t ev, process_data_t data));
	/*
	 * Overrides this method to run periodically.
	 * To stop this timer, return PT_ENDED, otherwise return 0.
	 */
	virtual PT_THREAD(run());
};

#endif /* _PT_TIMER_H_ */
