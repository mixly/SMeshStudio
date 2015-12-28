/*
****************************************************************************
* Copyright (c) 2015 Arduino srl. All right reserved.
*
* File : Ciao.h
* Date : 2015/09/17
* Revision : 0.0.1 $
* Author: andrea[at]arduino[dot]org
*
****************************************************************************

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef BRIDGE_H_
#define BRIDGE_H_

#include <Arduino.h>
#include <CiaoData.h>
#include <Stream.h>

class CiaoClass {
	public:
		CiaoClass(Stream &_stream);
		void begin();
		CiaoData read(String);
		void write( String protocol, String param1, String param2 = "", String param3 = "");
		void writeResponse( String protocol, String id, String param1="", String param2 = "", String param3 = "");
		CiaoData parse(String, String);

	private:
		void dropAll();
		Stream &stream;
		bool started;
};

// This subclass uses a serial port Stream
class SerialCiaoClass : public CiaoClass {
	public:
		SerialCiaoClass(HardwareSerial &_serial)
			: CiaoClass(_serial), serial(_serial) {
			// Empty
		}
		void begin( unsigned long baudrate = 250000) {
			serial.begin(baudrate);
			CiaoClass::begin();
		}

	private:
		HardwareSerial &serial;
};

void splitString(String, String, String[], int size);

extern SerialCiaoClass Ciao;

#endif /* BRIDGE_H_ */
