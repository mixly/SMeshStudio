/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestResource.cpp
 *
 *      Contiki REST resource wrapper.
 *
 *      Author: Long
 */

#include "RestResource.h"

/*
 * Instantiates a rest resource.
 *
 * \param url the url of this resource
 * \param attributes (optional) the link-format attributes
 */
RestResource::RestResource(const char *url, const char *attributes) {
	init(url, attributes);
}

/*
 * Instantiates.
 *
 * \param url the url of this resource
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
RestResource::RestResource(const char *url, const char *attributes,
		RestHandler GET, RestHandler POST, RestHandler PUT, RestHandler DELETE) :
		next(NULL), GET(GET), POST(POST), PUT(PUT), DELETE(DELETE) {
	init(url, attributes);
}

/*
 * Instantiates.
 *
 * \param url the url of this resource
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
RestResource::RestResource(const char *url, const char *attributes,
		restful_handler GET, restful_handler POST, restful_handler PUT, restful_handler DELETE) :
		next(NULL), GET(NULL), POST(NULL), PUT(NULL), DELETE(NULL) {
	init(url, attributes);
	resource.flags = (rest_resource_flags_t)(
			(GET ? METHOD_GET : 0) | (POST ? METHOD_POST : 0) |
			(PUT ? METHOD_PUT : 0) | (DELETE ? METHOD_DELETE : 0));
	resource.get_handler = GET;
	resource.post_handler = POST;
	resource.put_handler = PUT;
	resource.delete_handler = DELETE;
}

RestResource::~RestResource() {

}

/*
 * Initializes.
 */
void RestResource::init(const char *url, const char *attributes) {
	resource.next = NULL;
	resource.flags = (rest_resource_flags_t)(METHOD_GET | METHOD_POST | METHOD_PUT | METHOD_DELETE);
	resource.url = url;
	resource.attributes = attributes;
	resource.get_handler = NULL;
	resource.post_handler = NULL;
	resource.put_handler = NULL;
	resource.delete_handler = NULL;
	urlLen = strlen(url);
}

/*
 * Overrides this method to handle GET.
 */
void RestResource::get(RestRequest& request, RestResponse& response) {
	if (GET)
		GET(request, response);
	else
		response.setStatusCode(REST.status.METHOD_NOT_ALLOWED);
}

/*
 * Overrides this method to handle POST.
 */
void RestResource::post(RestRequest& request,RestResponse& response) {
	if (POST)
		POST(request, response);
	else
		response.setStatusCode(REST.status.METHOD_NOT_ALLOWED);
}

/*
 * Overrides this method to handle PUT.
 */
void RestResource::put(RestRequest& request, RestResponse& response) {
	if (PUT)
		PUT(request, response);
	else
		response.setStatusCode(REST.status.METHOD_NOT_ALLOWED);
}

/*
 * Overrides this method to handle DELETE.
 */
void RestResource::del(RestRequest& request, RestResponse& response) {
	if (DELETE)
		DELETE(request, response);
	else
		response.setStatusCode(REST.status.METHOD_NOT_ALLOWED);
}

/*
 * Instantiates a rest resource.
 *
 * \param url the url of this resource
 * \param attributes (optional) the link-format attributes
 */
EventResource::EventResource(const char *url, const char *attributes) :
		RestResource(url, attributes) {
	resource.flags = (rest_resource_flags_t) (resource.flags | IS_OBSERVABLE);
}

/*
 * Instantiates.
 *
 * \param url the url of this resource
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
EventResource::EventResource(const char *url, const char *attributes,
		RestHandler GET, RestHandler POST, RestHandler PUT, RestHandler DELETE) :
		RestResource(url, attributes, GET, POST, PUT, DELETE) {
	resource.flags = (rest_resource_flags_t) (resource.flags | IS_OBSERVABLE);
}

/*
 * Instantiates.
 *
 * \param url the url of this resource
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
EventResource::EventResource(const char *url, const char *attributes,
		restful_handler GET, restful_handler POST, restful_handler PUT, restful_handler DELETE) :
		RestResource(url, attributes, GET, POST, PUT, DELETE) {
	resource.flags = (rest_resource_flags_t) (resource.flags | IS_OBSERVABLE);
}

EventResource::~EventResource() {

}

/*
 * Notifies subscribers.
 */
void EventResource::notify(void) {
	REST.notify_subscribers(&resource);
}

/*
 * Factory function for making periodic_resource_t.
 */
static periodic_resource_t make_periodic_resource(restful_periodic_handler periodic_handler) {
	periodic_resource_t pr = { NULL, NULL, 0, { { 0 } }, periodic_handler };
	return pr;
}

/*
 * Instantiates a rest periodic resource.
 *
 * \param url the url of this resource
 * \param period the period of time in seconds
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
PeriodicResource::PeriodicResource(const char *url, float period,
		const char *attributes, RestHandler GET, RestHandler POST,
		RestHandler PUT, RestHandler DELETE, RestPeriodicHandler OBS) :
		EventResource(url, attributes, GET, POST, PUT, DELETE),
		periodicResource(make_periodic_resource(OBS)) {
	periodicResource.resource = &resource;
	setPeriod(period);
}

/*
 * Instantiates a rest periodic resource.
 *
 * \param url the url of this resource
 * \param period the period of time in seconds
 * \param attributes the link-format attributes
 * \param GET    the handler for GET
 * \param POST   (optional) the handler for POST
 * \param PUT    (optional) the handler for PUT
 * \param DELETE (optional) the handler for DELETE
 */
PeriodicResource::PeriodicResource(const char *url, float period,
		const char *attributes, restful_handler GET, restful_handler POST,
		restful_handler PUT, restful_handler DELETE, RestPeriodicHandler OBS) :
		EventResource(url, attributes, GET, POST, PUT, DELETE),
		periodicResource(make_periodic_resource(OBS)) {
	periodicResource.resource = &resource;
	setPeriod(period);
}

PeriodicResource::~PeriodicResource() {

}

/*
 * Sets the period of time in seconds.
 */
void PeriodicResource::setPeriod(float period) {
	this->period = period;
	if (period > 0) {
		resource.flags = (rest_resource_flags_t) (resource.flags | IS_OBSERVABLE | IS_PERIODIC);
		resource.periodic = &periodicResource;
		periodicResource.period = CLOCK_SECOND * period;
	}
}

/*
 * Gets the period of time in seconds.
 */
float PeriodicResource::getPeriod() {
	return period;
}

void PeriodicResource::get(RestRequest& request, RestResponse& response) {
	if (GET) {
		if (periodicResource.period > 0)
			response.setMaxAge(periodicResource.period / CLOCK_SECOND);
		GET(request, response);
	} else {
		response.setStatusCode(REST.status.METHOD_NOT_ALLOWED);
	}
}
