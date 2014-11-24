/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestResponse.cpp
 *
 *      Contiki REST response wrapper.
 *
 *      Author: Long
 */

#include <stdarg.h>
#include "RestResponse.h"

RestResponse::RestResponse(void *response, uint8_t *buffer, int32_t *offset) :
		response(response), buffer(buffer), offset(offset), payloadLength(0) {

}

RestResponse::~RestResponse() {
	flush();
}

/*
 * Prints a char.
 *
 * \param c the char to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(char c) {
	return printf("%c", c);
}

/*
 * Prints an int.
 *
 * \param i the int to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(int i) {
	return printf("%d", i);
}

/*
 * Prints an unsigned int.
 *
 * \param u the unsigned int to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(unsigned int u) {
	return printf("%u", u);
}

/*
 * Prints a float.
 *
 * \param f the float to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(float f) {
	return printf("%f", f);
}

/*
 * Prints a double.
 *
 * \param d the double to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(double d) {
	return printf("%lf", d);
}

/*
 * Prints a string.
 *
 * \param s the string to print
 * \return the actual count of written bytes.
 */
size_t RestResponse::print(const char *s) {
	return write(s, strlen(s));
}

/*
 * Prints a formatted string.
 */
size_t RestResponse::printf(const char *format, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, format);
	int printed = vsnprintf((char *)buffer + payloadLength, REST_MAX_CHUNK_SIZE - payloadLength, format, arg_ptr);
	va_end(arg_ptr);
	payloadLength += printed;
	return printed;
}

/*
 * Writes some bytes.
 *
 * \param data the bytes to write
 * \param length the length of data
 * \return the actual count of written bytes.
 *         This might be less than length if there is no enough space left in buffer.
 */
size_t RestResponse::write(const char *data, size_t length) {
	size_t copied = REST_MAX_CHUNK_SIZE - payloadLength;
	if (copied > length)
		copied = length;
	memcpy(this->buffer + payloadLength, data, copied);
	payloadLength += copied;
	return copied;
}

/*
 * Flushes the response.
 */
void RestResponse::flush() {
	REST.set_response_payload(response, buffer, payloadLength);
}

/*
 * Sets the status code of a response.
 */
int RestResponse::setStatusCode(unsigned int code) {
	return REST.set_response_status(response, code);
}

/*
 * Sets the Content-Type of a response.
 */
int RestResponse::setContentType(unsigned int contentType) {
	return REST.set_header_content_type(response, contentType);
}

/*
 * Sets the payload option of a response.
 */
int RestResponse::setPayload(const void *payload, size_t length) {
	setPayloadLength(length);
	return REST.set_response_payload(response, payload, length);
}

/*
 * Sets the payload length of a response.
 */
void RestResponse::setPayloadLength(size_t length) {
	payloadLength = length;
}

/*
 * Sets the Max-Age option of a response.
 */
int RestResponse::setMaxAge(uint32_t age) {
	return REST.set_header_max_age(response, age);
}

/*
 * Sets the ETag option of a response.
 */
int RestResponse::setETag(const uint8_t *etag, size_t length) {
	return REST.set_header_etag(response, etag, length);
}

/*
 * Sets the location option of a response.
 */
int RestResponse::setLocation(const char *location) {
	return REST.set_header_location(response, location);
}

int32_t RestResponse::getOffset() {
	return *offset;
}

void RestResponse::blockAppend(int32_t length) {
	*offset += length;
}

void RestResponse::blockComplete() {
	*offset = -1;
}
