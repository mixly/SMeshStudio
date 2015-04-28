/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestServer.h
 *
 *      Contiki REST server wrapper.
 *
 *      Author: Long
 */

#ifndef _REST_SERVER_H_
#define _REST_SERVER_H_

extern "C" {
#include "rest-engine.h"
}

/*
 * REST_SERVER_AUTOSTART_ENABLE specifies if RestServer should be allowed to start automatically.
 * If disabled, call RestServer.init() manually to start the rest server.
 */
#ifndef REST_SERVER_AUTOSTART_ENABLE
#define REST_SERVER_AUTOSTART_ENABLE AUTOSTART_ENABLE
#endif

class RestServerImpl;

#include "RestRequest.h"
#include "RestResponse.h"
#include "RestResource.h"

/*
 * The singleton instance of rest server.
 */
extern RestServerImpl RestServer;

/*
 * Use this macro to define the RestServer instance.
 */
#define RestServer(...) RestServerImpl RestServer(__VA_ARGS__, NULL);

/*
 * Represents a REST server.
 */
class RestServerImpl {
	/* static members */
private:
	/*
	 * Holds all RestResource instances.
	 */
	static RestResource *list;
	/*
	 * Finds a RestResource instance associated with the specified url.
	 */
	static RestResource* listFind(const char *url, int length);
	/*
	 * Finds a RestResource instance associated with the specified request.
	 */
	static RestResource* listFind(void *request);
	/*
	 * Adds a RestResource instance.
	 */
	static void listAdd(RestResource *item);
	/*
	 * Handles incoming GET.
	 */
	static void handleGet(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
	/*
	 * Handles incoming POST.
	 */
	static void handlePost(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
	/*
	 * Handles incoming PUT.
	 */
	static void handlePut(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
	/*
	 * Handles incoming DELETE.
	 */
	static void handleDelete(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
private:
	/*
	 * Makes a resource available.
	 */
	void activate(RestResource *resource);
public:
	/*
	 * Instantiates a REST server.
	 */
	RestServerImpl();
	/*
	 * Instantiates a REST server.
	 *
	 * \param resource ... the resources to add, should be terminated with a NULL.
	 */
	RestServerImpl(RestResource *resource, ...);
	/*
	 * Instantiates a REST server.
	 *
	 * \param count the total number of resources to add
	 * \param ... the resources to add
	 */
	RestServerImpl(int count, ...);
	~RestServerImpl();
	/*
	 * Initializes and starts the REST server.
	 */
	void init();
	/*
	 * Adds a resource. If the REST server has started already,
	 * the resource will be available at once.
	 */
	void addResource(RestResource *resource);
};

#endif /* _REST_SERVER_H_ */
