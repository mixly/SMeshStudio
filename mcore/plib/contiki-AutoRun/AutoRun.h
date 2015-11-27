/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * AutoRun.h
 *
 *      Run tasks automatically when system has booted.
 *
 *      Example:
 *
 *        void setup(void);
 *
 *        AutoRun(setup);
 *        // or
 *        // AutoRun run(setup);
 *
 *      Author: Long
 */

#ifndef _AUTO_RUN_H_
#define _AUTO_RUN_H_

extern "C" {
#include "contiki.h"
}

/*
 * AUTO_RUN_AUTOSTART_ENABLE specifies if AutoRun should be allowed to start automatically.
 * If disabled, call AutoRun::init() manually to start auto-run tasks.
 */
#ifndef AUTO_RUN_AUTOSTART_ENABLE
#define AUTO_RUN_AUTOSTART_ENABLE AUTOSTART_ENABLE
#endif

/*
 * Provides ability to run a task function automatically.
 */
class AutoRun {
	/* instance members */
private:
	AutoRun *next;
	/*
	 * the target function to run
	 */
	void (*target)(void);
public:
	/*
	 * Instantiates.
	 *
	 * \param target the target function to run
	 */
	AutoRun(void (*target)(void)) :
			target(target) {
		listAdd(this);
	}
	/*
	 * Calls the target function.
	 */
	void run();

	/* static members */
private:
	/*
	 * Holds all AutoRun instances.
	 */
	static AutoRun *list;
	/*
	 * Adds a AutoRun instance.
	 */
	static void listAdd(AutoRun *autorun);
public:
	/*
	 * Starts auto-run tasks.
	 */
	static void init();
};

/*
 * Defines a target function to run automatically.
 */
#define AutoRun(target) static AutoRun run_##target(target)

/*
 * Defines a task to run automatically.
 */
#define AutoTask(target)   \
static void auto_##target(void);  \
AutoRun(auto_##target);           \
static void auto_##target(void)

#endif /* _AUTO_RUN_H_ */
