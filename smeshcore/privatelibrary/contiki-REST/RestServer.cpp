/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestServer.cpp
 *
 *      Contiki REST server wrapper.
 *
 *      Author: Long
 */

#include <stdarg.h>
#include <string.h>
#include "RestServer.h"

//RestServerImpl RestServer;

RestResource* RestServerImpl::list = NULL;
static bool initialized = false;

#if REST_SERVER_AUTOSTART_ENABLE
PROCESS(rest_server_autostart_process, "RestServer Autostart Process");
AUTOSTART_PROCESSES(&rest_server_autostart_process);
PROCESS_THREAD(rest_server_autostart_process, ev, data)
{
	if (PROCESS_EVENT_INIT == ev) {
		RestServer.init();
	}
	return PT_ENDED;
}
#endif

/*
 * Instantiates a REST server.
 */
RestServerImpl::RestServerImpl() {

}

/*
 * Instantiates a REST server.
 *
 * \param resource ... the resources to add, should be terminated with a NULL.
 */
RestServerImpl::RestServerImpl(RestResource *resource, ...) {
	RestResource *res = resource;
	va_list arg_ptr;
	va_start(arg_ptr, resource);
	while (res != NULL) {
		addResource(res);
		res = va_arg(arg_ptr, RestResource*);
	}
	va_end(arg_ptr);
}

/*
 * Instantiates a REST server.
 *
 * \param count the total number of resources to add
 * \param ... the resources to add
 */
RestServerImpl::RestServerImpl(int count, ...) {
	RestResource *res;
	va_list arg_ptr;
	va_start(arg_ptr, count);
	for (int i = 0; i < count; i++) {
		res = va_arg(arg_ptr, RestResource*);
		addResource(res);
	}
	va_end(arg_ptr);
}

RestServerImpl::~RestServerImpl() {

}

/*
 * Initializes and starts the REST server.
 */
void RestServerImpl::init() {
	if (initialized)
		return;

	initialized = true;
	rest_init_engine();

	RestResource *res = list;
	while (res) {
		activate(res);
		res = res->next;
	}
}

/*
 * Adds a resource. If the REST server has started already,
 * the resource will be available at once.
 */
void RestServerImpl::addResource(RestResource *resource) {
	listAdd(resource);
	if (initialized)
		activate(resource);
}

/*
 * Makes a resource available.
 */
void RestServerImpl::activate(RestResource *resource) {
	if (resource->resource.get_handler == NULL)
		resource->resource.get_handler = handleGet;
	if (resource->resource.post_handler == NULL)
		resource->resource.post_handler = handlePost;
	if (resource->resource.put_handler == NULL)
		resource->resource.put_handler = handlePut;
	if (resource->resource.delete_handler == NULL)
		resource->resource.delete_handler = handleDelete;
	rest_activate_resource(&resource->resource, (char *)resource->resource.url);
}

/*
 * Handles incoming GET.
 */
void RestServerImpl::handleGet(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	RestResource *res = listFind(request);
	if (res != NULL) {
		RestRequest restRequest(request, preferred_size);
		RestResponse restResponse(response, buffer, offset);
		res->get(restRequest, restResponse);
	} else {
		REST.set_response_status(response, REST.status.NOT_FOUND);
	}
}

/*
 * Handles incoming POST.
 */
void RestServerImpl::handlePost(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	RestResource *res = listFind(request);
	if (res != NULL) {
		RestRequest restRequest(request, preferred_size);
		RestResponse restResponse(response, buffer, offset);
		res->post(restRequest, restResponse);
	} else {
		REST.set_response_status(response, REST.status.NOT_FOUND);
	}
}

/*
 * Handles incoming PUT.
 */
void RestServerImpl::handlePut(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	RestResource *res = listFind(request);
	if (res != NULL) {
		RestRequest restRequest(request, preferred_size);
		RestResponse restResponse(response, buffer, offset);
		res->put(restRequest, restResponse);
	} else {
		REST.set_response_status(response, REST.status.NOT_FOUND);
	}
}

/*
 * Handles incoming DELETE.
 */
void RestServerImpl::handleDelete(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
	RestResource *res = listFind(request);
	if (res != NULL) {
		RestRequest restRequest(request, preferred_size);
		RestResponse restResponse(response, buffer, offset);
		res->del(restRequest, restResponse);
	} else {
		REST.set_response_status(response, REST.status.NOT_FOUND);
	}
}

/*
 * Finds a RestResource instance associated with the specified request.
 */
RestResource* RestServerImpl::listFind(void *request) {
	if (request) {
		const char *url;
		int len = REST.get_url(request, &url);
		return listFind(url, len);
	} else {
		return NULL;
	}
}

/*
 * Finds a RestResource instance associated with the specified url.
 */
RestResource* RestServerImpl::listFind(const char *url, int length) {
	RestResource *res = list;
	while (res != NULL) {
		if (res->urlLen == length &&
				strncmp(res->resource.url, url, length) == 0)
			break;
		res = res->next;
	}
	return res;
}

/*
 * Adds a RestResource instance.
 */
void RestServerImpl::listAdd(RestResource *item) {
	if (list == NULL) {
		list = item;
	} else {
		RestResource *prev = list;
		while (prev->next != NULL)
			prev = prev->next;
		prev->next = item;
	}
}
