/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestResource.h
 *
 *      Contiki REST resource wrapper.
 *
 *      Author: Long
 */

#ifndef _REST_RESOURCE_H_
#define _REST_RESOURCE_H_

extern "C" {
#include "rest-engine.h"
}

/*
 * CC_NO_VIRTUAL_DESTRUCTOR specifies whether virtual destructors is disabled or not.
 * Virtual destructors are disabled by default for NOT being supported in AVR GCC.
 */
#ifndef CC_NO_VIRTUAL_DESTRUCTOR
#define CC_NO_VIRTUAL_DESTRUCTOR 1
#endif

#include "RestRequest.h"
#include "RestResponse.h"

typedef void(*RestHandler)(RestRequest&, RestResponse&);
typedef restful_periodic_handler RestPeriodicHandler;

/*
 * Represents a REST resource.
 */
class RestResource {
private:
	RestResource *next;
	friend class RestServerImpl;
protected:
	/*
	 * the associated Contiki REST resource
	 */
	resource_t resource;
	int urlLen;
public:
	/*
	 * the handler for GET
	 */
	RestHandler GET;
	/*
	 * the handler for POST
	 */
	RestHandler POST;
	/*
	 * the handler for PUT
	 */
	RestHandler PUT;
	/*
	 * the handler for DELETE
	 */
	RestHandler DELETE;
public:
	/*
	 * Instantiates a REST resource.
	 *
	 * \param url the url of this resource
	 * \param attributes (optional) the link-format attributes
	 */
	RestResource(const char *url, const char *attributes = "");
	/*
	 * Instantiates a REST resource.
	 *
	 * \param url the url of this resource
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	RestResource(const char *url, const char *attributes,
			RestHandler GET, RestHandler POST = NULL,
			RestHandler PUT = NULL, RestHandler DELETE = NULL);
	/*
	 * Instantiates a REST resource.
	 *
	 * \param url the url of this resource
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	RestResource(const char *url, const char *attributes,
			restful_handler GET, restful_handler POST = NULL,
			restful_handler PUT = NULL, restful_handler DELETE = NULL);
#if !CC_NO_VIRTUAL_DESTRUCTOR
	virtual
#endif
	~RestResource();
private:
	/*
	 * Initializes.
	 */
	void init(const char *url, const char *attributes);
protected:
	/*
	 * Overrides this method to handle GET.
	 */
	virtual void get(RestRequest& request, RestResponse& response);
	/*
	 * Overrides this method to handle POST.
	 */
	virtual void post(RestRequest& request, RestResponse& response);
	/*
	 * Overrides this method to handle PUT.
	 */
	virtual void put(RestRequest& request, RestResponse& response);
	/*
	 * Overrides this method to handle DELETE.
	 */
	virtual void del(RestRequest& request, RestResponse& response);
};

/*
 * Represents a REST event resource.
 */
class EventResource : public RestResource {
public:
	/*
	 * Instantiates a REST event resource.
	 *
	 * \param url the url of this resource
	 * \param attributes (optional) the link-format attributes
	 */
	EventResource(const char *url, const char *attributes = "");
	/*
	 * Instantiates a REST event resource.
	 *
	 * \param url the url of this resource
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	EventResource(const char *url, const char *attributes,
			RestHandler GET, RestHandler POST = NULL,
			RestHandler PUT = NULL, RestHandler DELETE = NULL);
	/*
	 * Instantiates a REST event resource.
	 *
	 * \param url the url of this resource
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	EventResource(const char *url, const char *attributes,
			restful_handler GET, restful_handler POST = NULL,
			restful_handler PUT = NULL, restful_handler DELETE = NULL);
#if !CC_NO_VIRTUAL_DESTRUCTOR
	virtual
#endif
	~EventResource();
	/*
	 * Notifies subscribers.
	 */
	void notify(void);
};

/*
 * Represents a REST periodic resource.
 */
class PeriodicResource : public EventResource {
private:
	/*
	 * the period of time in seconds
	 */
	float period;
	/*
	 * the associated Contiki REST periodic resource
	 */
	periodic_resource_t periodicResource;
public:
	/*
	 * Instantiates a REST periodic resource.
	 *
	 * \param url the url of this resource
	 * \param period the period of time in seconds
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	PeriodicResource(const char *url, float period, const char *attributes,
			RestHandler GET, RestHandler POST = NULL,
			RestHandler PUT = NULL, RestHandler DELETE = NULL,
			RestPeriodicHandler OBS = NULL);
	/*
	 * Instantiates a REST periodic resource.
	 *
	 * \param url the url of this resource
	 * \param period the period of time in seconds
	 * \param attributes the link-format attributes
	 * \param GET    the handler for GET
	 * \param POST   (optional) the handler for POST
	 * \param PUT    (optional) the handler for PUT
	 * \param DELETE (optional) the handler for DELETE
	 */
	PeriodicResource(const char *url, float period, const char *attributes,
			restful_handler GET, restful_handler POST = NULL,
			restful_handler PUT = NULL, restful_handler DELETE = NULL,
			RestPeriodicHandler OBS = NULL);
#if !CC_NO_VIRTUAL_DESTRUCTOR
	virtual
#endif
	~PeriodicResource();
	/*
	 * Sets the period of time in seconds.
	 */
	void setPeriod(float period);
	/*
	 * Gets the period of time in seconds.
	 */
	float getPeriod();
protected:
	/*
	 * FIXME NOT work in er-coap now since it removes all context info in notifications.
	 */
	virtual void get(RestRequest& request, RestResponse& response);
};

#endif /* _REST_RESOURCE_H_ */
