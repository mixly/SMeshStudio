/* 1-Wire-Master Library
*  Copyright (c) 2010 Jan Achterhold
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

/* 1-Wire-Master, currently without multiple slave support */ 

#include "mbed.h"
#include "OneWire.h"

OneWire::OneWire(PinName _owpin) : owpin(_owpin) {
    owpin.mode(PullUp);
}

OneWire::~OneWire() {
};

// Reads 1-Wire ROM-Code
void OneWire::getRomCode(char *rc) {

	this->busInit();
	
    // Sends ROM-Code sending command to the bus
    this->writeByte(0x33);
	
    // Reads 8-byte ROM-Code
    for (int i=0; i<=7; i++) {
        rc[i] = this->readByte();
    }
}

// Commands device to do sth.
void OneWire::cmdDevice(char *rc, char cmd) {
    // Reset
    this->busInit();
    // Matches ROM
    this->writeByte(0x55);
    for (int i=0; i<=7; i++) {
        this->writeByte(rc[i]);
    }
    // Writes command
    this->writeByte(cmd);
}

// Gets data from the bus
void OneWire::getData(char *data, int bytes) {
    for (int i=0;i<=bytes-1;i++) {
    data[i] = this->readByte();
    }
}

// Writes a bit to the bus
void OneWire::writeBit(int bit) {
    this->owpin.output();
	if (bit==1)
	{
    this->owpin = 0;
    wait_us(10);
    this->owpin = 1;
    wait_us(55);
 }
	else
	{
		this->owpin = 0;
    wait_us(65);
    this->owpin = 1;
    wait_us(5);
 
	}
}

// Reads a bit from the bus
int OneWire::readBit() {
    int pin;
    this->owpin.output();
    this->owpin = 0;
    wait_us(3);
    this->owpin = 1;
    wait_us(10);
    this->owpin.input();
    pin = this->owpin;
    this->owpin.output();
    wait_us(53);
    return pin;
}

// Writes a byte to the bus
void OneWire::writeByte(unsigned char by) {
    int b;
    int i;
    for (i=0; i<=7; i++) {
        b = by & 0x01;
        this->writeBit(b);
        by = by >> 1;
    }
}

// Reads Byte from the bus
unsigned char OneWire::readByte(void) {
    unsigned char i;
    unsigned char wert = 0;
    for (i=0; i<8; i++) {
        if (this->readBit()) wert |=0x01 << i;
    }
    return(wert);
}


// Initializes bus
int OneWire::busInit() {
    int r;
    this->owpin.output();
    this->owpin = 0;
    wait_us(540);
    this->owpin = 1;
    this->owpin.input();
    wait_us(65);
    r = this->owpin;
    this->owpin.output();
    wait_us(540);
    return r;
}