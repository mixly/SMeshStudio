/*
 * Copyright (c) 2011-2014, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * RestResponse.h
 *
 *      Contiki REST response wrapper.
 *
 *      Author: Long
 */

#ifndef _REST_RESPONSE_H_
#define _REST_RESPONSE_H_

extern "C" {
#include "rest-engine.h"
}

/*
 * Represents a REST response.
 */
class RestResponse {
public:
	/*
	 * the actual outgoing response
	 */
	void* const response;
private:
	/*
	 * the payload buffer
	 */
	uint8_t* const buffer;
	/*
	 * the offset for blockwise transfer
	 */
	int32_t *offset;
	/*
	 * the length of payload
	 */
	size_t payloadLength;
public:
	/*
	 * Instantiates.
	 *
	 * \param response the actual outgoing response
	 * \param buffer the payload buffer
	 * \param offset the offset for blockwise transfer
	 */
	RestResponse(void *response, uint8_t *buffer, int32_t *offset);
	~RestResponse();
	/*
	 * Writes some bytes.
	 *
	 * \param data the bytes to write
	 * \param length the length of data
	 * \return the actual count of written bytes.
	 *         This might be less than length if there is no enough space left in buffer.
	 */
	size_t write(const char *data, size_t length);
	/*
	 * Prints a string.
	 *
	 * \param s the string to print
	 * \return the actual count of written bytes.
	 */
	size_t print(const char *s);
	/*
	 * Prints a char.
	 *
	 * \param c the char to print
	 * \return the actual count of written bytes.
	 */
	size_t print(char c);
	/*
	 * Prints an int.
	 *
	 * \param i the int to print
	 * \return the actual count of written bytes.
	 */
	size_t print(int i);
	/*
	 * Prints an unsigned int.
	 *
	 * \param u the unsigned int to print
	 * \return the actual count of written bytes.
	 */
	size_t print(unsigned int u);
	/*
	 * Prints a float.
	 *
	 * \param f the float to print
	 * \return the actual count of written bytes.
	 */
	size_t print(float f);
	/*
	 * Prints a double.
	 *
	 * \param d the double to print
	 * \return the actual count of written bytes.
	 */
	size_t print(double d);
	/*
	 * Prints a formatted string.
	 */
	size_t printf(const char *format, ...);
	/*
	 * Flushes the response.
	 */
	void flush();
	/*
	 * Sets the status code of a response.
	 */
	int setStatusCode(unsigned int code);
	/*
	 * Sets the Content-Type of a response.
	 */
	int setContentType(unsigned int contentType);
	/*
	 * Sets the payload option of a response.
	 */
	int setPayload(const void *payload, size_t length);
	/*
	 * Sets the payload length of a response.
	 */
	void setPayloadLength(size_t length);
	/*
	 * Sets the Max-Age option of a response.
	 */
	int setMaxAge(uint32_t age);
	/*
	 * Sets the ETag option of a response.
	 */
	int setETag(const uint8_t *etag, size_t length);
	/*
	 * Sets the location option of a response.
	 */
	int setLocation(const char *location);

	int32_t getOffset();
	void blockAppend(int32_t length);
	void blockComplete();
};

#endif /* _REST_RESPONSE_H_ */
