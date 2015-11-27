/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * PtTimer.cpp
 *
 *      Timer based on PtProcess.
 *
 *      Author: Long
 */

#include "PtTimer.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*
 * Instantiates a timer.
 *
 * \param interval the interval to timeout
 * \param delay (optional) the delay before starting timing
 * \param name (optional) the name of this timer
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
//PtTimer::PtTimer(clock_time_t interval, clock_time_t delay,
//		const char *name, bool autostart) :
//		PtProcess(name, autostart), interval(interval),
//		delay(delay), target(NULL) {
//
//}

/*
 * Instantiates a timer.
 *
 * \param interval the interval to timeout in seconds
 * \param delay (optional) the delay before starting timing in seconds
 * \param name (optional) the name of this timer
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
PtTimer::PtTimer(float interval, float delay,
		const char *name, bool autostart) :
		PtProcess(name, autostart), interval(CLOCK_CONF_SECOND * interval),
		delay(CLOCK_CONF_SECOND * delay), target(NULL) {

}

/*
 * Instantiates a timer.
 *
 * \param target the target function to run periodically
 * \param interval the interval to timeout
 * \param delay (optional) the delay before starting timing
 * \param name (optional) the name of this timer
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
//PtTimer::PtTimer(PT_THREAD((*target)()), clock_time_t interval,
//		clock_time_t delay, const char *name, bool autostart) :
//		PtProcess(name, autostart), interval(interval),
//		delay(delay), target(target) {
//
//}

/*
 * Instantiates a timer.
 *
 * \param target the target function to run periodically
 * \param interval the interval to timeout in seconds
 * \param delay (optional) the delay before starting timing in seconds
 * \param name (optional) the name of this timer
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
PtTimer::PtTimer(PT_THREAD((*target)()), float interval,
		float delay, const char *name, bool autostart) :
		PtProcess(name, autostart), interval(CLOCK_CONF_SECOND * interval),
		delay(CLOCK_CONF_SECOND * delay), target(target) {

}

PtTimer::~PtTimer() {
	etimer_stop(&timer);
}

PT_THREAD(PtTimer::run(struct pt *process_pt, process_event_t ev, process_data_t data)) {
	char result;

	PROCESS_BEGIN();

	if (delay > 0) {
		PRINTF("PtTimer: prepare for delay...\n");
		etimer_set(&timer, delay);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		PRINTF("PtTimer: complete delay.\n");
	}

	etimer_set(&timer, interval);

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		result = this->run();
		if (PT_ENDED == result || PT_EXITED == result) {
			PRINTF("PtTimer: end timing.\n");
			break;
		}
		etimer_reset(&timer);
	}

	PROCESS_END();
}

/*
 * Overrides this method to run periodically.
 * To stop this timer, return PT_ENDED, otherwise return 0.
 */
PT_THREAD(PtTimer::run()) {
	return target ? target() : PT_ENDED;
}
