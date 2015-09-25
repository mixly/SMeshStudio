/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Platform Specific part of Hardware interface Layer
 * ----------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define CPLUSPLUS
#ifdef __cplusplus
extern "C" {
#endif
#include "jstimer.h"
#include "jshardware.h"
#include "jsutils.h"
#include "jsparse.h"
#include "jsinteractive.h"
//#include "serial_api.h"
#ifdef __cplusplus
}
#endif
// --------------------------------------------------- MBED DEFS
#include "mbed.h"

uint32_t lastSystemTime;
uint32_t nowTime;
uint64_t systemTime;
Ticker utilTicker;

jsGpio *mbedGpios;

#if DEVICE_SERIAL
jsSerial *mbedSerials;
#endif

#if DEVICE_ANALOGIN
jsAnalogIn *mbedAnalogIns;
#endif

#if DEVICE_ANALOGOUT
jsAnalogOut *mbedAnalogOuts;
#endif

#if DEVICE_PWMOUT
jsPwmout *mbedPwmouts;
#endif

#if DEVICE_INTERRUPTIN
jsInterruptIn *mbedInterruptIns;
#endif

#if DEVICE_SPI
jsSpi *mbedSpis;
#endif

#if DEVICE_I2C
jsI2c *mbedI2cs;
#endif
static long long DEVICE_INITIALISED_FLAGS = 0L;

#ifdef __cplusplus
extern "C" {
#endif


//---------------mbed gpio
bool findGpioInMbedGpios(jsGpio **jsgpio,Pin pin){

	*jsgpio = mbedGpios;
	while(*jsgpio){
		if((*jsgpio)->gpio.pin == pin)  return true;
		(*jsgpio) = (*jsgpio) ->next;
	}

	return false;
}

void addToMbedGpios(jsGpio *jsgpio){
	jsgpio->next = mbedGpios;
	mbedGpios = jsgpio;
}


bool jshGetPinStateIsManual(Pin pin) {
	jsGpio *jsgpio;
	if(findGpioInMbedGpios(&jsgpio,pin))
		return jsgpio ->isStateManual;
	return false;
}

void jshSetPinStateIsManual(Pin pin, bool manual) {
	jsGpio *jsgpio;
	if(findGpioInMbedGpios(&jsgpio,pin))
		jsgpio ->isStateManual = manual;
}


void jshPinSetState(Pin pin, JshPinState state) {
	bool out = JSHPINSTATE_IS_OUTPUT(state);
	bool pullup = state==JSHPINSTATE_GPIO_OUT_OPENDRAIN || state==JSHPINSTATE_GPIO_IN_PULLUP || state==JSHPINSTATE_USART_IN;
	bool pulldown = state==JSHPINSTATE_GPIO_IN_PULLDOWN;
	bool opendrain = JSHPINSTATE_IS_OPENDRAIN(state);

	jsGpio *jsgpio;
	if(!findGpioInMbedGpios(&jsgpio,pin)){
		jsgpio = (jsGpio*)malloc(sizeof(jsGpio));
		if(!jsgpio)  return ;
		gpio_init(&jsgpio ->gpio, pin);
		jsgpio->isStateManual = false;
		addToMbedGpios(jsgpio);
	}

	if(out){
		gpio_dir(&jsgpio ->gpio, PIN_OUTPUT);
#ifndef HASNOOPENDRAIN
		if(opendrain)   gpio_mode(&jsgpio ->gpio,OpenDrain);
#endif
	}else {
		gpio_dir(&jsgpio ->gpio, PIN_INPUT);
		if(pullup){
			gpio_mode(&jsgpio ->gpio,PullUp);
		}else if(pulldown){
#ifndef HASNOPULLDOWN
			gpio_mode(&jsgpio ->gpio,PullDown);
#endif
		}else {
			gpio_mode(&jsgpio ->gpio,PullDefault);
		}
	}

}

//------------------digitalOut/In
void jshPinSetValue(Pin pin, bool value) {
	jsGpio *jsgpio;
	if(findGpioInMbedGpios(&jsgpio,pin)){
		gpio_write(&jsgpio ->gpio, value);
	}
}
bool jshPinGetValue(Pin pin) {
	jsGpio *jsgpio;
	if(findGpioInMbedGpios(&jsgpio,pin))
		return gpio_read(&jsgpio ->gpio);

	return false;
}

//------------------device initialised or not
void jshSetDeviceInitialised(IOEventFlags device, bool isInit) {
	long long mask = 1L << (int)device;
	if (isInit) {
		DEVICE_INITIALISED_FLAGS |= mask;
	} else {
		DEVICE_INITIALISED_FLAGS &= ~mask;
	}
}

bool jshIsDeviceInitialised(IOEventFlags device) {
	long long mask = 1L << (int)device;
	return (DEVICE_INITIALISED_FLAGS & mask) != 0L;
}

// --------------------mbed serial
bool findSerialInMbedSerials(jsSerial **jsserial,IOEventFlags device){
#if DEVICE_SERIAL

	*jsserial = mbedSerials;
	while(*jsserial){
		if((*jsserial)->flag == device)  return true;
		(*jsserial) = (*jsserial) ->next;
	}
#endif
	return false;
}

JshSerialDeviceState *getSerialDeviceStates(IOEventFlags device){
	jsSerial *jsserial;
	if(findSerialInMbedSerials(&jsserial,device)){
		return &jsserial->state;
	}

	return 0;
}

void mbedSerialIRQ(uint32_t id, SerialIrq event) {
#if DEVICE_SERIAL
	IOEventFlags device = (IOEventFlags)id;
	jsSerial *jsserial;
	if(findSerialInMbedSerials(&jsserial,device)){
		if (event == RxIrq) {
			if (serial_readable(&jsserial->serial)){
				//jshPinOutput(LED1, !jshPinGetValue(LED1));
				jshPushIOCharEvent(device, (char)serial_getc(&jsserial->serial));
			}
		}
		if (event == TxIrq) {
			int c = jshGetCharToTransmit(device);
			if (c >= 0) {
				serial_putc(&jsserial->serial, c);
			} else
				serial_irq_set(&jsserial->serial, TxIrq, 0);
		}
	}
#endif
}

void addToMbedSerials(jsSerial *jsserial){
#if DEVICE_SERIAL
	jsserial->next = mbedSerials;
	mbedSerials = jsserial;
#endif
}

void jshResetSerial() {
#if DEVICE_SERIAL
	jsSerial *jsserial;
	if(!jshIsDeviceInitialised(EV_STDIOSERIAL)){
		jsserial = (jsSerial *)malloc(sizeof(jsSerial));
		if(!jsserial)  return ;
		addToMbedSerials(jsserial);

		serial_init(&jsserial->serial,USBTX, USBRX);
		jsserial->state = SDS_NONE;
		jsserial->flag = EV_STDIOSERIAL;
		serial_irq_handler(&jsserial->serial, &mbedSerialIRQ, EV_STDIOSERIAL);
		serial_irq_set(&jsserial->serial, RxIrq, 1);

		jshSetDeviceInitialised(EV_STDIOSERIAL, true);

	}
#endif
}

void jshUSARTSetup(IOEventFlags device, JshUSARTInfo *inf) {
#if DEVICE_SERIAL
	if (!DEVICE_IS_USART(device)) return;

	jsSerial *jsserial;
	if(jshIsDeviceInitialised(device)){
		findSerialInMbedSerials(&jsserial,device);
	}else{
		jshSetDeviceInitialised(device, true);
		jsserial = (jsSerial *)malloc(sizeof(jsSerial));
		if(!jsserial)  return ;
		addToMbedSerials(jsserial);
	}

	serial_init(&jsserial->serial, inf->pinTX,inf->pinRX);
	jsserial->flag = device;
	jsserial->state = SDS_NONE;
	jshSetFlowControlEnabled(device, inf->xOnXOff);
	serial_baud(&jsserial->serial,inf->baudRate);
	serial_format(&jsserial->serial,inf->bytesize,(SerialParity)inf->parity,inf->stopbits);
	serial_irq_handler(&jsserial->serial, &mbedSerialIRQ, (uint32_t)device);
	serial_irq_set(&jsserial->serial, RxIrq, 1);
#endif
}

/** Kick a device into action (if required). For instance we may need
 * to set up interrupts */
void jshUSARTKick(IOEventFlags device) {
#if DEVICE_SERIAL
	jsSerial *jsserial;
	findSerialInMbedSerials(&jsserial,device);
	int c = jshGetCharToTransmit(device);

	if (c >= 0) {
		serial_irq_set(&jsserial->serial, TxIrq, 1);
		serial_putc(&jsserial->serial, c);
	}
#endif
}


//------------------AnalogOut/In
#if DEVICE_ANALOGIN
bool findAnalogInInMbedAnalogIns(jsAnalogIn **jsanalogIn,Pin pin){

	*jsanalogIn = mbedAnalogIns;
	while(*jsanalogIn){
		if((*jsanalogIn)->pin == pin)  return true;
		(*jsanalogIn) = (*jsanalogIn) ->next;
	}

	return false;
}

void addToMbedAnalogIns(jsAnalogIn *jsanalogIn){
	jsanalogIn->next = mbedAnalogIns;
	mbedAnalogIns = jsanalogIn;
}
#endif

//todo : should change a name like jshPinAnalogRead()
JsVarFloat jshPinAnalog(Pin pin) {
#if DEVICE_ANALOGIN
	if(pin == NC)  return 0;
	jshSetPinStateIsManual(pin, false);
	jsAnalogIn *jsanalogIn;
	if(!findAnalogInInMbedAnalogIns(&jsanalogIn,pin)){
		jsanalogIn = (jsAnalogIn*)malloc(sizeof(jsAnalogIn));
		if(!jsanalogIn)  return 0;
		analogin_init(&jsanalogIn->analogIn, pin);
		jsanalogIn->pin = pin;
		addToMbedAnalogIns(jsanalogIn);
	}

	return (double)analogin_read(&jsanalogIn->analogIn);
#endif
	return 0;
}

int jshPinAnalogFast(Pin pin){
#if DEVICE_ANALOGIN
	if(pin == NC)  return 0;
	jshSetPinStateIsManual(pin, false);

	jsAnalogIn *jsanalogIn;
	if(!findAnalogInInMbedAnalogIns(&jsanalogIn,pin)){
		jsanalogIn = (jsAnalogIn*)malloc(sizeof(jsAnalogIn));
		if(!jsanalogIn)  return 0;
		analogin_init(&jsanalogIn->analogIn, pin);
		jsanalogIn->pin = pin;
		addToMbedAnalogIns(jsanalogIn);
	}

	return analogin_read_u16(&jsanalogIn->analogIn);
#endif
	return 0;
}

#if DEVICE_ANALOGOUT
bool findAnalogOutMbedAnalogOuts(jsAnalogOut **jsanalogOut,Pin pin){

	*jsanalogOut = mbedAnalogOuts;
	while(*jsanalogOut){
		if((*jsanalogOut)->pin == pin)  return true;
		(*jsanalogOut) = (*jsanalogOut) ->next;
	}

	return false;
}

void addToMbedAnalogOuts(jsAnalogOut *jsanalogOut){
	jsanalogOut->next = mbedAnalogOuts;
	mbedAnalogOuts = jsanalogOut;
}
#endif

#if DEVICE_PWMOUT
bool findPwmoutMbedPwmouts(jsPwmout **jspwmout,Pin pin){

	*jspwmout = mbedPwmouts;
	while(*jspwmout){
		if((*jspwmout)->pin == pin)  return true;
		(*jspwmout) = (*jspwmout) ->next;
	}

	return false;
}

void addToMbedPwmouts(jsPwmout *jspwmout){
	jspwmout->next = mbedPwmouts;
	mbedPwmouts = jspwmout;
}
#endif

void jshPinAnalogOutput(Pin pin, JsVarFloat value, JsVarFloat freq) {// if freq<=0, the default is used
	if(value < 0) value = 0;
	if(value > 1) value = 1;
	if(pin == NC)  return ;

	if(freq <= 0){  //using DAC to deal with it

#if DEVICE_ANALOGOUT
		jshSetPinStateIsManual(pin, false);
		jsAnalogOut *jsanalogOut;
		if(!findAnalogOutMbedAnalogOuts(&jsanalogOut,pin)){
			jsanalogOut = (jsAnalogOut*)malloc(sizeof(jsAnalogOut));
			if(!jsanalogOut)  return ;
			analogout_init(&jsanalogOut->analogOut, pin);
			jsanalogOut->pin = pin;
			addToMbedAnalogOuts(jsanalogOut);

		}
		analogout_write(&jsanalogOut->analogOut,(float)value);
#endif

	}else {  //using PWM to deal with it

#if DEVICE_PWMOUT
		jshSetPinStateIsManual(pin, false);
		jsPwmout *jspwmout;
		if(!findPwmoutMbedPwmouts(&jspwmout,pin)){
			jspwmout = (jsPwmout*)malloc(sizeof(jsPwmout));
			if(!jspwmout)   return ;
			pwmout_init(&jspwmout->pwmout, pin);
			jspwmout->pin = pin;
			addToMbedPwmouts(jspwmout);
		}
		pwmout_period(&jspwmout->pwmout,(float)(1 / freq));
		pwmout_write(&jspwmout->pwmout,(float)value);
#endif

	}

}


//---------------InterruptIn
#if DEVICE_INTERRUPTIN
bool findGpioIrqInMbedGpioIrqs(jsInterruptIn **jsgpioIrq,Pin pin){

	*jsgpioIrq = mbedInterruptIns;
	while(*jsgpioIrq){
		if((*jsgpioIrq)->pin == pin)  return true;
		(*jsgpioIrq) = (*jsgpioIrq) ->next;
	}

	return false;
}

void addToMbedGpioIrqs(jsInterruptIn *jsgpioIrq){
	jsgpioIrq->next = mbedInterruptIns;
	mbedInterruptIns = jsgpioIrq;
}


void pinIrqHandler(uint32_t id, gpio_irq_event event) {
	jshPushIOWatchEvent(EV_EXTI,id);
}
#endif
/// Set a callback function to be called when an event occurs
void jshSetEventCallback(Pin pin, JshEventCallbackCallback callback) {
#if DEVICE_INTERRUPTIN
	jsInterruptIn *jsgpioIrq;
	if(findGpioIrqInMbedGpioIrqs(&jsgpioIrq,pin)){
		jsgpioIrq->callback = callback;
	}
#endif
}

bool callWatchPinEventCallback(Pin pin,bool state){
#if DEVICE_INTERRUPTIN
	jsInterruptIn *jsgpioIrq;
	if(findGpioIrqInMbedGpioIrqs(&jsgpioIrq,pin)){
		if(jsgpioIrq->callback){
			jsgpioIrq->callback(state);
			return true;
		}
	}
#endif
	return false;
}


IOEventFlags jshPinWatch(Pin pin, bool shouldWatch) {
#if DEVICE_INTERRUPTIN
	if (jshIsPinValid(pin)) {

		if(pin == NC)  return EV_NONE;
		//jsiConsolePrintf("pin = %d\n",pin);

		jsInterruptIn *jsgpioIrq;
		if(!findGpioIrqInMbedGpioIrqs(&jsgpioIrq,pin)){
			//jsiConsolePrintf("111\n");
			jsgpioIrq = (jsInterruptIn*)malloc(sizeof(jsInterruptIn));
			if(!jsgpioIrq)   return EV_NONE;
			jsgpioIrq->pin = pin;
			jsgpioIrq->callback = 0;
			addToMbedGpioIrqs(jsgpioIrq);
		}

		if(shouldWatch){
			if (!jshGetPinStateIsManual(pin)){
				//jsiConsolePrintf("222\n");
				jshPinSetState(pin,JSHPINSTATE_GPIO_IN_PULLUP);
			}
		}

		if(shouldWatch){
			gpio_irq_init(&jsgpioIrq->gpioIrq, pin, pinIrqHandler, pin);
			gpio_irq_set(&jsgpioIrq->gpioIrq, IRQ_FALL, 1);
			gpio_irq_set(&jsgpioIrq->gpioIrq, IRQ_RISE, 1);
		}else{
			gpio_irq_set(&jsgpioIrq->gpioIrq, IRQ_FALL, 0);
			gpio_irq_set(&jsgpioIrq->gpioIrq, IRQ_RISE, 0);
		}
		return (IOEventFlags)(~(EV_EXTI + pin) + 1);
	}else jsExceptionHere(JSET_ERROR, "Invalid pin!");
#endif
	return EV_NONE;
}

//-------------i2c
bool findI2cInMbedI2cs(jsI2c **jsi2c,IOEventFlags device){
#if DEVICE_I2C

	*jsi2c = mbedI2cs;
	while(*jsi2c){
		if((*jsi2c)->flag == device)  return true;
		(*jsi2c) = (*jsi2c) ->next;
	}
#endif
	return false;
}

void addToMbedI2cs(jsI2c *jsi2c){
#if DEVICE_I2C
	jsi2c->next = mbedI2cs;
	mbedI2cs = jsi2c;
#endif
}

void jshI2CSetup(IOEventFlags device, JshI2CInfo *inf) {
#if DEVICE_I2C
	jsI2c *jsi2c;
	if(jshIsDeviceInitialised(device)){
		findI2cInMbedI2cs(&jsi2c,device);
	}else{
		jshSetDeviceInitialised(device, true);
		jsi2c = (jsI2c *)malloc(sizeof(jsI2c));
		if(!jsi2c)  return ;
		addToMbedI2cs(jsi2c);
	}
	i2c_init(&jsi2c->i2c, inf->pinSDA, inf->pinSCL);
	i2c_frequency(&jsi2c->i2c, inf->bitrate);
	jsi2c->flag = device;
#endif
}

void jshI2CWrite(IOEventFlags device, unsigned char address, int nBytes, const unsigned char *data, bool sendStop) {
#if DEVICE_I2C
	if(jshIsDeviceInitialised(device)){
		jsI2c *jsi2c;
		findI2cInMbedI2cs(&jsi2c,device);
		int send = sendStop?1:0;
		i2c_write(&jsi2c->i2c, (int)address, (char *)data, nBytes, send);
	}
#endif
}

void jshI2CRead(IOEventFlags device, unsigned char address, int nBytes, unsigned char *data, bool sendStop) {
#if DEVICE_I2C
	if(jshIsDeviceInitialised(device)){
		jsI2c *jsi2c;
		findI2cInMbedI2cs(&jsi2c,device);
		int send = sendStop?1:0;
		i2c_read(&jsi2c->i2c, (int)address, (char *)data, nBytes, send);
	}
#endif

}

//-----------------spi
bool findSpiInMbedSpis(jsSpi **jsspi,IOEventFlags device){
#if DEVICE_SPI

	*jsspi = mbedSpis;
	while(*jsspi){
		if((*jsspi)->flag == device)  return true;
		(*jsspi) = (*jsspi) ->next;
	}
#endif
	return false;
}

void addToMbedSpis(jsSpi *jsspi){
#if DEVICE_SPI
	jsspi->next = mbedSpis;
	mbedSpis = jsspi;
#endif
}

void jshSPISetup(IOEventFlags device, JshSPIInfo *inf) {
#if DEVICE_SPI
	jsSpi *jsspi;
	if(jshIsDeviceInitialised(device)){
		findSpiInMbedSpis(&jsspi,device);
	}else{
		jshSetDeviceInitialised(device, true);
		jsspi = (jsSpi *)malloc(sizeof(jsSpi));
		if(!jsspi)  return ;
		addToMbedSpis(jsspi);
	}
	spi_init(&jsspi->spi, inf->pinMOSI, inf->pinMISO, inf->pinSCK, NC);
	spi_format(&jsspi->spi, inf->bits, inf->spiMode, 0);
	spi_frequency(&jsspi->spi, inf->baudRate);
	jsspi->flag = device;
#endif
}

int jshSPISend(IOEventFlags device, int data) {
#if DEVICE_SPI
	if(data < 0) return -1;
	if(jshIsDeviceInitialised(device)){
		jsSpi *jsspi;
		findSpiInMbedSpis(&jsspi,device);
		return spi_master_write(&jsspi->spi, data);
	}
	return -1;
#endif
}


void jshSPIWait(IOEventFlags device){
}


void jshSPISet16(IOEventFlags device, bool is16){

}

void jshSPISend16(IOEventFlags device, int data){

}

// for non-blocking IO
void jshInit() {
	//init some device vars
	mbedGpios = 0;

#if DEVICE_SERIAL
	mbedSerials = 0;
#endif

#if DEVICE_ANALOGIN
	mbedAnalogIns = 0;
#endif

#if DEVICE_ANALOGOUT
	mbedAnalogOuts = 0;
#endif

#if DEVICE_PWMOUT
	mbedPwmouts = 0;
#endif

#if DEVICE_INTERRUPTIN
	mbedInterruptIns = 0;
#endif

#if DEVICE_SPI
	mbedSpis = 0;
#endif

#if DEVICE_I2C
	mbedI2cs = 0;
#endif

	//reset serial
	jshResetSerial();

	//init system time
	lastSystemTime = us_ticker_read();
	systemTime = 0;
}


void jshReset(){
	//free and reinit some device vars
	DEVICE_INITIALISED_FLAGS = 0;

	if(mbedGpios){
		jsGpio *jsgpio = mbedGpios;
		while(mbedGpios){
			mbedGpios = mbedGpios->next;
			free(jsgpio);
			jsgpio = mbedGpios;
		}
		mbedGpios = 0;
	}


#if DEVICE_SERIAL
	if(mbedSerials){
		jsSerial *jsserial = mbedSerials;
		while(mbedSerials){
			mbedSerials = mbedSerials->next;
			free(jsserial);
			jsserial = mbedSerials;
		}
		mbedSerials = 0;
	}
#endif

#if DEVICE_ANALOGIN
	if(mbedAnalogIns){
		jsAnalogIn *jsanalogIn = mbedAnalogIns;
		while(mbedAnalogIns){
			mbedAnalogIns = mbedAnalogIns->next;
			free(jsanalogIn);
			jsanalogIn = mbedAnalogIns;
		}
		mbedAnalogIns = 0;
	}
#endif

#if DEVICE_ANALOGOUT
	if(mbedAnalogOuts){
		jsAnalogOut *jsanalogOut = mbedAnalogOuts;
		while(mbedAnalogOuts){
			mbedAnalogOuts = mbedAnalogOuts->next;
			free(jsanalogOut);
			jsanalogOut = mbedAnalogOuts;
		}
		mbedAnalogOuts = 0;
	}
#endif

#if DEVICE_PWMOUT
	if(mbedPwmouts){
		jsPwmout *jspwmout = mbedPwmouts;
		while(mbedPwmouts){
			mbedPwmouts = mbedPwmouts->next;
			free(jspwmout);
			jspwmout = mbedPwmouts;
		}
		mbedPwmouts = 0;
	}
#endif

#if DEVICE_INTERRUPTIN
	if(mbedInterruptIns){
		jsInterruptIn *jsinterruptIn = mbedInterruptIns;
		while(mbedInterruptIns){
			mbedInterruptIns = mbedInterruptIns->next;
			free(jsinterruptIn);
			jsinterruptIn = mbedInterruptIns;
		}
		mbedInterruptIns = 0;
	}
#endif

	//reset serial
	jshResetSerial();

	//init system time
	lastSystemTime = us_ticker_read();
	systemTime = 0;
}


void jshIdle() {

#ifdef USB
	static bool wasUSBConnected = false;
	bool USBConnected = jshIsUSBSERIALConnected();
	if (wasUSBConnected != USBConnected) {
		wasUSBConnected = USBConnected;
		if (USBConnected)
			jsiSetConsoleDevice(EV_USBSERIAL);
		else {
			if (jsiGetConsoleDevice() == EV_USBSERIAL)
				jsiSetConsoleDevice(DEFAULT_CONSOLE_DEVICE);
			jshTransmitClearDevice(EV_USBSERIAL); // clear the transmit queue
		}
	}
#else
	jsiSetConsoleDevice(DEFAULT_CONSOLE_DEVICE);
#endif
	/*while (serial_readable(&mbedSerial[0])>0)
		jshPushIOCharEvent(EV_SERIAL1, serial_getc(&mbedSerial[0]));*/
}

// ----------------------------------------------------------------------------

//todo:the function shouldn't be implemented in mbed.
int jshGetSerialNumber(unsigned char *data, int maxChars) {
	return 0;
}

// ----------------------------------------------------------------------------

void jshInterruptOff() {
	__disable_irq();
}

void jshInterruptOn() {
	__enable_irq();
}

void jshDelayMicroseconds(int microsec) {
	wait_us(microsec);
}


bool jshIsUSBSERIALConnected() {
	return false;
}

JsSysTime jshGetTimeFromMilliseconds(JsVarFloat ms) {
	return (JsSysTime)(ms*1000);
}

JsVarFloat jshGetMillisecondsFromTime(JsSysTime time) {
	return ((JsVarFloat)time)/1000;
}

JsSysTime jshGetSystemTime() {
	int t ;
	nowTime = us_ticker_read();

	if(nowTime >= lastSystemTime) t = nowTime - lastSystemTime;
	else t = (int)(0x100000000LL -  lastSystemTime + nowTime) ;

	systemTime += t;

	lastSystemTime = nowTime;

	return systemTime;
}

void jshSetSystemTime(JsSysTime time) {
	systemTime =  time;
	lastSystemTime = time;
}

// ----------------------------------------------------------------------------


void jshPinPulse(Pin pin, bool pulsePolarity, JsVarFloat pulseTime) {
	// ---- USE TIMER FOR PULSE
	if (!jshIsPinValid(pin)) {
		jsExceptionHere(JSET_ERROR, "Invalid pin!");
		return;
	}
	if (pulseTime<=0) {
		// just wait for everything to complete
		jstUtilTimerWaitEmpty();
		return;
	} else {
		// find out if we already had a timer scheduled
		UtilTimerTask task;
		if (!jstGetLastPinTimerTask(pin, &task)) {
			// no timer - just start the pulse now!
			jshPinOutput(pin, pulsePolarity);
			task.time = jshGetSystemTime();
		}
		// Now set the end of the pulse to happen on a timer
		//jsiConsolePrintf("out the at time : %f\n",pulseTime);
		jstPinOutputAtTime(task.time + jshGetTimeFromMilliseconds(pulseTime), &pin, 1, !pulsePolarity);
	}
}


//zzl
JshPinState jshPinGetState(Pin pin){
	return (JshPinState)0;
}

JshPinFunction jshGetCurrentPinFunction(Pin pin){
	return (JshPinFunction)0;
}

void jshSetOutputValue(JshPinFunction func, int value){

}

//zzl
void jshEnableWatchDog(JsVarFloat timeout){

}






void jshSaveToFlash() {

}

//todo:fix me in mbed
void jshLoadFromFlash() {
	/*
	unsigned int dataSize = jsvGetMemoryTotal() * sizeof(JsVar);
	jsiConsolePrintf("Loading from flash...\n");

	JsVar *firstData = jsvLock(1);
	uint32_t *basePtr = (uint32_t *)firstData;
	jsvUnLock(firstData);

	memcpy(basePtr, (int*)FLASH_SAVED_CODE_START, dataSize);
	 */
}

//todo:fix me in mbed
bool jshFlashContainsCode() {
	//return (*(int*)FLASH_MAGIC_LOCATION) == (int)FLASH_MAGIC;
	return false;
}

/// Enter simple sleep mode (can be woken up by interrupts). Returns true on success
bool jshSleep(JsSysTime timeUntilWake) {
	//__WFI(); // Wait for Interrupt
	return true;
}

void jshUtilTimerDisable() {
	utilTicker.detach();
}

void jshUtilTimerReschedule(JsSysTime period) {
	//jsiConsolePrintf("restart:: %d\n",period);
	utilTicker.attach_us(jstUtilTimerInterruptHandler,period);
}

void jshUtilTimerStart(JsSysTime period) {
	//jsiConsolePrintf("start:: %d\n",period);
	utilTicker.attach_us(jstUtilTimerInterruptHandler,period);
}

JsVarFloat jshReadTemperature() { return NAN; };
JsVarFloat jshReadVRef()  { return NAN; };

// ----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
