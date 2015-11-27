/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * CallbackTimer.cpp
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

#include "CallbackTimer.h"

/*
 * The default instance of CallbackTimer.
 */
CallbackTimer CTimer;

/*
 * Instantiates a callback timer.
 */
CallbackTimer::CallbackTimer() :
		period(0), autoReset(false), context(NULL), target(NULL) {

}

/*
 * Instantiates a callback timer.
 *
 * \param callback the target function to call when timeout
 * \param period the period before timeout
 * \param autoReset (optional) auto reset the timer or not. The default value is false.
 * \param context (optional) the context. The default value is NULL.
 */
CallbackTimer::CallbackTimer(void (*callback)(void *ctx), float period, bool autoReset, void *context) :
		period(CLOCK_CONF_SECOND * period), autoReset(autoReset), context(context), target(callback) {

}

CallbackTimer::~CallbackTimer() {
	stop();
}

/*
 * Begins timing with specified period and callback.
 *
 * \param period the period before timeout
 * \param callback the target function to call when timeout
 * \param context (optional) the context. The default value is NULL.
 */
void CallbackTimer::setTimeout(float period, void (*callback)(void *ctx), void *context) {
	this->period = CLOCK_CONF_SECOND * period;
	this->target = callback;
	this->context = context;
	start();
}

/*
 * Starts this timer.
 */
void CallbackTimer::start() {
	ctimer_set(&timer, period, ctimer_callback, this);
}

/*
 * Stops this timer.
 */
void CallbackTimer::stop() {
	ctimer_stop(&timer);
}

void CallbackTimer::ctimer_callback(void *ctx) {
	CallbackTimer *timer = (CallbackTimer *)ctx;
	if (timer->target)
		timer->target(timer->context);
	if (timer->autoReset)
		ctimer_reset(&timer->timer);
}
