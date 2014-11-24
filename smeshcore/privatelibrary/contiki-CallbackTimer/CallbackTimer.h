/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * CallbackTimer.h
 *
 *      Timer based on Contiki ctimer.
 *
 *      Usage:
 *
 *        CTimer.setTimeout(period, callback[, context]);
 *
 *        // or
 *
 *        CallbackTimer timer(callback, period[, autoReset, context]);
 *        timer.start();
 *
 *      Author: Long
 */

#ifndef _CALLBACK_TIMER_H_
#define _CALLBACK_TIMER_H_

extern "C" {
#include "contiki.h"
}

/*
 * Timer based on Contiki ctimer.
 */
class CallbackTimer {
private:
	static void ctimer_callback(void *ctx);
	/*
	 * the associated Contiki ctimer
	 */
	struct ctimer timer;
protected:
	/*
	 * the period before timeout
	 */
	clock_time_t period;
	/*
	 * auto reset the timer or not
	 */
	bool autoReset;
	/*
	 * the context
	 */
	void *context;
	/*
	 * the target callback function
	 */
	void (*target)(void *ctx);
public:
	/*
	 * Instantiates a callback timer.
	 */
	CallbackTimer();
	/*
	 * Instantiates a callback timer.
	 *
	 * \param callback the target function to call when timeout
	 * \param period the period before timeout
	 * \param autoReset (optional) auto reset the timer or not. The default value is false.
	 * \param context (optional) the context. The default value is NULL.
	 */
	CallbackTimer(void (*callback)(void *ctx), float period, bool autoReset = false, void *context = NULL);
	~CallbackTimer();
	/*
	 * Begins timing with specified period and callback.
	 *
	 * \param period the period before timeout
	 * \param callback the target function to call when timeout
	 * \param context (optional) the context. The default value is NULL.
	 */
	void setTimeout(float period, void (*callback)(void *ctx), void *context = NULL);
	/*
	 * Starts this timer.
	 */
	void start();
	/*
	 * Stops this timer.
	 */
	void stop();
};

/*
 * The default instance of CallbackTimer.
 */
extern CallbackTimer CTimer;

#endif /* _CALLBACK_TIMER_H_ */
