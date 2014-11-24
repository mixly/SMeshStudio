/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * PtProcess.h
 *
 *      Contiki proto-thread process wrapper.
 *
 *      Author: Long
 */

#ifndef _PT_PROCESS_H_
#define _PT_PROCESS_H_

extern "C" {
#include "contiki.h"
}

/*
 * PT_PROCESS_AUTOSTART_ENABLE specifies if PtProcess should be allowed to start automatically.
 * If disabled, call PtProcess::startAutostart() manually to start auto-start processes.
 */
#ifndef PT_PROCESS_AUTOSTART_ENABLE
#define PT_PROCESS_AUTOSTART_ENABLE AUTOSTART_ENABLE
#endif

/*
 * CC_NO_VIRTUAL_DESTRUCTOR specifies whether virtual destructors is disabled or not.
 * Virtual destructors are disabled by default for NOT being supported in AVR GCC.
 */
#ifndef CC_NO_VIRTUAL_DESTRUCTOR
#define CC_NO_VIRTUAL_DESTRUCTOR 1
#endif

/*
 * Wraps Contiki proto-thread process.
 */
class PtProcess {
	/* static members */
private:
	/*
	 * Holds all PtProcess instances.
	 */
	static PtProcess *list;
	/*
	 * Finds a PtProcess instance associated with the specified protothread.
	 */
	static PtProcess* listFind(struct pt *process_pt);
	/*
	 * Adds a PtProcess instance.
	 */
	static void listAdd(PtProcess *proc);
	/*
	 * Routes process events.
	 */
	PROCESS_THREAD(pt_process_manager, ev, data);
public:
	/*
	 * Starts auto-start processes.
	 */
	static void startAutostart();

	/* instance members */
private:
	PtProcess *next;
	/*
	 * the target PT_THREAD function to run
	 */
	PT_THREAD((*target)(struct pt*, process_event_t, process_data_t));
protected:
	/*
	 * the associated Contiki process
	 */
	struct process process;
	bool autostart;
private:
	void init(const char *name, bool autostart, PT_THREAD((*target)(struct pt*, process_event_t, process_data_t)));
public:
	/*
	 * Instantiates a process.
	 *
	 * \param name (optional) the process name
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
	PtProcess(const char *name = NULL, bool autostart = true);
	/*
	 * Instantiates a process.
	 *
	 * \param target the target PT_THREAD function to run
	 * \param name (optional) the process name
	 * \param autostart (optional) true if to be started automatically, otherwise false. The default value is true.
	 */
	PtProcess(PT_THREAD((*target)(struct pt*, process_event_t, process_data_t)), const char *name = NULL, bool autostart = true);
#if !CC_NO_VIRTUAL_DESTRUCTOR
	virtual
#endif
	~PtProcess();
	/*
	 * Starts this process.
	 */
	void start(const char *arg = NULL);
protected:
	/*
	 * Overrides this method to run this process with the given parameters.
	 */
	virtual PT_THREAD(run(struct pt *process_pt, process_event_t ev, process_data_t data));
};

#endif /* _PT_PROCESS_H_ */
