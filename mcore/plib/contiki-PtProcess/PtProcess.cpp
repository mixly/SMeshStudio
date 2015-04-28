/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * PtProcess.cpp
 *
 *      Contiki proto-thread process wrapper.
 *
 *      Author: Long
 */

#include "PtProcess.h"

/*
 * Holds all PtProcess instances.
 */
PtProcess* PtProcess::list = NULL;

#if PT_PROCESS_AUTOSTART_ENABLE
PROCESS(pt_process_autostart_process, "PtProcess Autostart Process");
AUTOSTART_PROCESSES(&pt_process_autostart_process);
PROCESS_THREAD(pt_process_autostart_process, ev, data)
{
	PROCESS_BEGIN();
	if (PROCESS_EVENT_INIT == ev) {
		PtProcess::startAutostart();
	}
	PROCESS_END();
}
#endif

/*
 * Starts auto-start processes.
 */
void PtProcess::startAutostart() {
	PtProcess *t = list;
	while (t) {
		if (t->autostart)
			t->start();
		t = t->next;
	}
}

/*
 * Adds a PtProcess instance.
 */
void PtProcess::listAdd(PtProcess *item) {
	if (list == NULL) {
		list = item;
	} else {
		PtProcess *prev = list;
		while (prev->next != NULL)
			prev = prev->next;
		prev->next = item;
	}
}

/*
 * Finds a PtProcess instance associated with the specified protothread.
 */
PtProcess* PtProcess::listFind(struct pt *process_pt) {
	PtProcess *q = list;
	for (q = list; &q->process.pt != process_pt && q != NULL; q = q->next)
		;
	return q;
}

/*
 * Routes process events.
 */
PT_THREAD(PtProcess::process_thread_pt_process_manager(struct pt *process_pt, process_event_t ev, process_data_t data)) {
	PtProcess *proc = listFind(process_pt);
	if (proc) {
		return proc->run(process_pt, ev, data);
	} else {
		return PT_ENDED;
	}
}

/*
 * Instantiates a process.
 *
 * \param name (optional) the process name
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
PtProcess::PtProcess(const char *name, bool autostart) {
	init(name, autostart, NULL);
}

/*
 * Instantiates a process.
 *
 * \param target the target PT_THREAD function to run
 * \param name (optional) the process name
 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
 */
PtProcess::PtProcess(
		PT_THREAD((*target)(struct pt *process_pt, process_event_t ev, process_data_t data)),
		const char *name, bool autostart) {
	init(name, autostart, target);
}

PtProcess::~PtProcess() {
	process_exit(&process);
}

void PtProcess::init(const char *name, bool autostart, PT_THREAD((*target)(struct pt *, process_event_t, process_data_t))) {
	this->next = NULL;
	this->target = target;
	this->autostart = autostart;

	this->process.next = NULL;
#if !PROCESS_CONF_NO_PROCESS_NAMES
	this->process.name = name;
#endif
	this->process.thread = process_thread_pt_process_manager;

	listAdd(this);
}

/*
 * Starts this process.
 */
void PtProcess::start(const char *arg) {
	process_start(&process, arg);
}

/*
 * Overrides this method to run this process with the given parameters.
 */
PT_THREAD(PtProcess::run(struct pt *process_pt, process_event_t ev, process_data_t data)) {
	return target ? target(process_pt, ev, data) : PT_ENDED;
}
