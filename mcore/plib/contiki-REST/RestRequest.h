/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestRequest.h
 *
 *      Contiki REST request wrapper.
 *
 *      Author: Long
 */

#ifndef _REST_REQUEST_H_
#define _REST_REQUEST_H_

extern "C" {
#include "rest-engine.h"
}

/*
 * Represents a REST request.
 */
class RestRequest {
public:
	/*
	 * the actual incoming request
	 */
	void* const request;
	/*
	 * the preferred size for outgoing payload
	 */
	const uint16_t preferredSize;
public:
	/*
	 * Instantiates.
	 *
	 * \param request the actual incoming request
	 * \param preferredSize the preferred size for outgoing payload
	 */
	RestRequest(void* request, uint16_t preferredSize);
	~RestRequest();
	/*
	 * Gets request URI path.
	 */
	int getPath(const char **url);
	/*
	 * Gets the method of a request.
	 */
	rest_resource_flags_t getMethod();
	/*
	 * Gets the content-type of a request.
	 */
	int getContentType(unsigned int *content_format);
	/*
	 * Gets the Accept types of a request.
	 */
	int getAccept(unsigned int *accept);
	/*
	 * Gets the Length option of a request.
	 */
	int getLength(uint32_t *size);
	/*
	 * Gets the Max-Age option of a request.
	 */
	int getMaxAge(uint32_t *age);
	/*
	 * Gets the If-Match option of a request.
	 */
	int getIfMatch(const uint8_t **etag);
	/*
	 * Gets the If-Match option of a request.
	 */
	int getIfNoneMatch();
	/*
	 * Gets the Host option of a request.
	 */
	int getHost(const char **host);
	/*
	 * Gets the payload option of a request.
	 */
	int getPayload(const uint8_t **payload);
	/*
	 * Gets the query string of a request.
	 */
	int getQuery(const char **value);
	/*
	 * Gets the value of a request query key-value pair.
	 */
	int getQueryVariable(const char *name, const char **value);
	/*
	 * Gets the value of a request POST key-value pair.
	 */
	int getPostVariable(const char *name, const char **value);
	/*
	 * Gets the value of a request parameter key-value pair in query or POST body.
	 */
	int getVariable(const char *name, const char **value);
};

#endif /* _REST_REQUEST_H_ */
