/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestRequest.cpp
 *
 *      Contiki REST request wrapper.
 *
 *      Author: Long
 */

#include "RestRequest.h"


/*
 * Instantiates.
 *
 * \param request the actual incoming request
 * \param preferredSize the preferred size for outgoing payload
 */
RestRequest::RestRequest(void* request, uint16_t preferredSize) :
		request(request), preferredSize(preferredSize) {

}

RestRequest::~RestRequest() {

}

/*
 * Gets request URI path.
 */
int RestRequest::getPath(const char **url) {
	return REST.get_url(request, url);
}

/*
 * Gets the method of a request.
 */
rest_resource_flags_t RestRequest::getMethod() {
	return REST.get_method_type(request);
}

/*
 * Gets the content-type of a request.
 */
int RestRequest::getContentType(unsigned int *content_format) {
	return REST.get_header_content_type(request, content_format);
}

/*
 * Gets the Accept types of a request.
 */
int RestRequest::getAccept(unsigned int *accept) {
	return REST.get_header_accept(request, accept);
}

/*
 * Gets the Length option of a request.
 */
int RestRequest::getLength(uint32_t *size) {
	return REST.get_header_length(request, size);
}

/*
 * Gets the Max-Age option of a request.
 */
int RestRequest::getMaxAge(uint32_t *age) {
	return REST.get_header_max_age(request, age);
}

/*
 * Gets the If-Match option of a request.
 */
int RestRequest::getIfMatch(const uint8_t **etag) {
	return REST.get_header_if_match(request, etag);
}

/*
 * Gets the If-Match option of a request.
 */
int RestRequest::getIfNoneMatch() {
	return REST.get_header_if_none_match(request);
}

/*
 * Gets the Host option of a request.
 */
int RestRequest::getHost(const char **host) {
	return REST.get_header_host(request, host);
}

/*
 * Gets the payload option of a request.
 */
int RestRequest::getPayload(const uint8_t **payload) {
	return REST.get_request_payload(request, payload);
}

/*
 * Gets the query string of a request.
 */
int RestRequest::getQuery(const char **value) {
	return REST.get_query(request, value);
}

/*
 * Gets the value of a request query key-value pair.
 */
int RestRequest::getQueryVariable(const char *name, const char **value) {
	return REST.get_query_variable(request, name, value);
}

/*
 * Gets the value of a request POST key-value pair.
 */
int RestRequest::getPostVariable(const char *name, const char **value) {
	return REST.get_post_variable(request, name, value);
}

/*
 * Gets the value of a request parameter key-value pair in query or POST body.
 */
int RestRequest::getVariable(const char *name, const char **value) {
	int len = getQueryVariable(name, value);
	return len ? len : getPostVariable(name, value);
}
