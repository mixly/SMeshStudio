/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * AutoRun.cpp
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

#include "AutoRun.h"

/*
 * Holds all AutoRun instances.
 */
AutoRun* AutoRun::list = NULL;

#if AUTO_RUN_AUTOSTART_ENABLE
PROCESS(auto_run_autostart_process, "AutoRun Autostart Process");
AUTOSTART_PROCESSES(&auto_run_autostart_process);
PROCESS_THREAD(auto_run_autostart_process, ev, data)
{
	PROCESS_BEGIN();
	if (PROCESS_EVENT_INIT == ev) {
		AutoRun::init();
	}
	PROCESS_END();
}
#endif

/*
 * Starts auto-run tasks.
 */
void AutoRun::init() {
	AutoRun *t = list;
	while (t) {
		t->run();
		t = t->next;
	}
}

/*
 * Adds a AutoRun instance.
 */
void AutoRun::listAdd(AutoRun *item) {
	if (list == NULL) {
		list = item;
	} else {
		AutoRun *prev = list;
		while (prev->next != NULL)
			prev = prev->next;
		prev->next = item;
	}
}

/*
 * Calls the target function.
 */
void AutoRun::run() {
	if (target)
		target();
}
