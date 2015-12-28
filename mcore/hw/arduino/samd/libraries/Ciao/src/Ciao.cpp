/*
****************************************************************************
* Copyright (c) 2015 Arduino srl. All right reserved.
*
* File : Ciao.cpp
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

#include "Ciao.h"

CiaoClass::CiaoClass(Stream &_stream) :
  stream(_stream), started(false) {
  // Empty
}

void CiaoClass::begin() {
	
	String start;
	String stop;

  // Wait for U-boot to finish startup
	do {
		dropAll();
		delay(1000);
	}while (stream.available() > 0);

	stream.println("ciao;r;status");					//check the bridge python status	
	String status = stream.readStringUntil(END_TX_CHAR);
	if(status == "1;running"){						
		do{												//kill a istance of the bridge.py
			stream.println("ciao;w;quit");
			stop = stream.readStringUntil(END_TX_CHAR);
			delay(3000);
		}while(stop != "1;done");
    }
	do{ 
		stream.print(F("run-ciao\n"));				//start bridge python
		stream.readStringUntil(END_TX_CHAR);
		delay(3000);
		stream.println("ciao;r;status");				//check if bridge python is running
		start = stream.readStringUntil(END_TX_CHAR);
	}while (start != "1;running");
}

CiaoData CiaoClass::read( String connector) {
	
	CiaoData data;
	stream.println(connector + ";r");					// send read request 
	String message = stream.readStringUntil(END_TX_CHAR);
	data = parse(message, ";");
	return data;
}

void CiaoClass::write(String connector, String param1, String param2 , String param3 ) {

	CiaoData data;
	if (param1 != ""){
		stream.print(connector+";w;");
		stream.print(param1);
	}
	if (param2 != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(param2);
	}
	if (param3 != ""){
		stream.print(DATA_SPLIT_CHAR);
		stream.print(param3);
	}
	stream.println();
	String mess_receive = stream.readStringUntil(END_TX_CHAR);
	data = parse(mess_receive, ";");
	
}

void CiaoClass::writeResponse( String connector, String id, String param1, String param2 , String param3) {

	CiaoData data;
	if(id.length()> ID_SIZE_TX){
		if (param1 != ""){
			stream.print(connector+";wr;");
			stream.print(id+";");
			stream.print(param1);
		}
		if (param2 != ""){
			stream.print(DATA_SPLIT_CHAR);
			stream.print(param2);
		}
		if (param3 != ""){
			stream.print(DATA_SPLIT_CHAR);
			stream.print(param3);
		}
		stream.println();
		String mess_receive = stream.readStringUntil(END_TX_CHAR);
		data = parse(mess_receive, ";");
	}
}

CiaoData CiaoClass::parse(String message, String split){
	String status;
	CiaoData data;
	int statusIndex = message.indexOf(split);
	int messIndex = message.indexOf(split, statusIndex+1);
	status = message.substring(0, statusIndex);				//message status (0: no message; 1:data ready; -1:error)

	if(status == ID_READY && message.substring(statusIndex+1, messIndex) != "done"){
		data.msg_split[0]=message.substring(statusIndex+1, messIndex);
		data.parseMessage(message.substring(messIndex+1));	//parsing data received
	}
	else{
		data.msg_split[0] = status;
		data.msg_split[2] = message.substring(statusIndex+1, messIndex);
	}
	return data;
}

void CiaoClass::dropAll() {
  while (stream.available() > 0) {
    stream.read();
  }
}

//fuction to split command (ex: digital/13/1)
void splitString(String command, String split, String msg[], int size){		
	  
	for(int a=0;a< size ;a++)		//initialize array element to -1
		msg[a]= ID_ERROR;
	char buf[command.length()+1];
	command.toCharArray(buf, command.length()+1);
	char* array;
	const char splt[1]= {split.charAt(0)};
	int i=0;
	array = strtok (buf,splt);
	while (array != NULL){  
		msg[i]=String(array);
		array = strtok (NULL, splt);
		i++; 
	}
}

// Ciao instance
#ifdef __AVR_ATmega32U4__
// Leonardo variants (where HardwareSerial is Serial1)
SerialCiaoClass Ciao(Serial1);
#else
SerialCiaoClass Ciao(Serial);
#endif
