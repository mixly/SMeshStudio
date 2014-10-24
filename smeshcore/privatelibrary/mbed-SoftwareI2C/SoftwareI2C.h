/*
 * mbed Library to use a software master i2c interface on any GPIO pins
 * Copyright (c) 2012 Christopher Pepper
 * Released under the MIT License: http://mbed.org/license/mit
 */

#ifndef _SOFTWARE_I2C_H_
#define _SOFTWARE_I2C_H_

#include "mbed.h"

/**
 * @brief SoftwareI2C class
 */

class SoftwareI2C {
public:
	SoftwareI2C(PinName sda, PinName scl);
	~SoftwareI2C();

	void read(char device_address, char* data, char data_bytes);
	void write(char device_address, char* data,  char data_bytes);
	void write(char device_address, char byte);
	void randomRead(char device_address, char start_address, char* data, char data_bytes);
	void randomWrite(char device_address, char start_address, char* data, char data_bytes);
	void randomWrite(char device_address, char start_address, char byte);

	char read8(char device_address, char start_address);
	uint16_t read16(char device_address, char start_address);
	uint32_t read24(char device_address, char start_address);
	uint32_t read32(char device_address, char start_address);

	void setDeviceAddress(char address){
		_device_address = address;
	}

	void setFrequency(uint32_t frequency){
		_frequency_delay = 1000000 / frequency;
	}

	inline void initialise() {
		_scl.output();
		_sda.output();

		_sda = 1;
		_scl = 0;
		wait_us(_frequency_delay);

		for ( int n = 0; n <= 3; ++n ) {
			stop();
		}
	}

private:
	inline void start() {
		_sda.output();
		wait_us(_frequency_delay);
		_scl = 1;
		_sda = 1;
		wait_us(_frequency_delay);
		_sda = 0;
		wait_us(_frequency_delay);
		_scl = 0;
		wait_us(_frequency_delay);
	}

	inline void stop() {
		_sda.output();
		wait_us(_frequency_delay);
		_sda = 0;
		wait_us(_frequency_delay);
		_scl = 1;
		wait_us(_frequency_delay);
		_sda = 1;
	}

	inline void putByte(char byte) {
		_sda.output();
		for ( int n = 8; n > 0; --n) {
			wait_us(_frequency_delay);
			_sda = byte & (1 << (n-1));
			_scl = 1;
			wait_us(_frequency_delay);
			_scl = 0;
		}
		_sda = 1;
	}

	inline char getByte() {
		char byte = 0;

		_sda.input();          //release the data line
		_sda.mode(OpenDrain);

		wait_us(_frequency_delay);

		for ( int n = 8; n > 0; --n ) {
			_scl=1;            //set clock high
			wait_us(_frequency_delay);
			byte |= _sda << (n-1); //read the bit
			wait_us(_frequency_delay);
			_scl=0;            //set clock low
			wait_us(_frequency_delay);
		}

		_sda.output();         //take data line back

		return byte;
	}

	inline void giveAck() {
		_sda.output();
		wait_us(_frequency_delay);
		_sda = 0;
		_scl = 1;
		wait_us(_frequency_delay);
		_scl = 0;
		_sda = 1;

	}

	inline bool getAck() {
		_sda.output();
		_sda = 1;
		_scl = 1;
		_sda.input();
		_sda.mode(OpenDrain);
		wait_us(_frequency_delay);
		_scl = 0;

		if(_sda != 0){return false;}

		wait_us(_frequency_delay);
		return true;
	}

	DigitalInOut _sda;
	DigitalInOut _scl;

	char _device_address;
	uint32_t _frequency_delay;
};

#endif
