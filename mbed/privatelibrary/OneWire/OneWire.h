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

/** 1-Wire-Master, currently without multiple slave support 
*
* Example:
* @code
* // Reads a DS1820-1-Wire temperature sensor and stores one value at the local file system.
* #include "mbed.h"
* #include "OneWire.h"

* OneWire ow(p8);
* LocalFileSystem local("local");             // Create the local filesystem under the name "local"

* int main() {
*    FILE *fp = fopen("/local/out.txt", "w");
*    char romcode[8];             // Array for ROM-Code 
*    char scratchpad[9];          // Array for Scratchpad
*
*    ow.getRomCode(romcode);      // Get ROM-Code
*    ow.cmdDevice(romcode, 0x44); // Convert Temp
*    wait_ms(500);                // Wait for conversion
*    ow.cmdDevice(romcode, 0xBE); // Command for sending scratchpad
*    ow.getData(scratchpad, 9);   // Read Scratchpad
*    int tmp = (((scratchpad[0]&0xFE)>>1)&0x7F);  // Calculate temperature
*    
*    fprintf(fp, "Temp.: %d", tmp);  // Write temperature to the file "out.txt"
*    fclose(fp);
* }
* @endcode
*/

#ifndef MBED_ONEWIRE_H
#define MBED_ONEWIRE_H
#include "mbed.h"


class OneWire {
public:
    /** Creates an object "OneWire" connected to the specified DigitalInOut pin (p5 - p30)
    * @param PinName _owpin     The pin name the 1-Wire-Device is connected to
    */
    OneWire(PinName _owpin);
    ~OneWire();
    
     /** Initializes the bus.
    *   @return     Response of the bus device.
                    Can be used to detect whether there's a device connected or not.
    */
    int busInit();
    
    /** Fills the specified char array "rc" with the ROM-Code of the plugged device.
    *   There's only one device on the bus possible.
    *   Multislave support will be added later.
    *   @param char *rc     Pointer to the ROM-Code-Array
    */
    void getRomCode(char *rc);
    
    /** Command 1-Wire-Device with ROM-Code "rc" to perform the instruction "cmd".
    *   @param char *rc     Pointer to the ROM-Code-Array
        @param char cmd     1-Wire command
    */
    void cmdDevice(char *rc, char cmd);
    
    /** Puts "bytes" bytes of data on the bus to array "data".
    *   @param char *data     Pointer to the Data-Array
        @param int bytes      Number of bytes to be read
    */
    void getData(char *data, int bytes);
    
    /** Writes the bit "bit" the bus.
    *   @param int bit  Bit (0 or 1) to be written
    */
    void writeBit(int bit);
    
    /** Reads a bit from the bus and returns as integer.
    *   @return int Bit (0 or 1) from the bus device
    */
    int readBit();
    
    /** Writes the byte "byte" to the bus.
    *   @param unsigned char by  Byte to be written
    */
    void writeByte(unsigned char by);
    
    /** Reads a byte from the bus and returns as char.
    *   @return unsigned char Byte from the bus device.
    */
    unsigned char readByte();
    
    
protected:
    DigitalInOut owpin;
};

#endif