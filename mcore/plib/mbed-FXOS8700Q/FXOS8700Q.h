/* Copyright (c) 2010-2011 mbed.org, MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FXOS8700Q_H
#define FXOS8700Q_H

#include "mbed.h"
#include "MotionSensor.h"
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7


/**
* MMA8451Q accelerometer example
*
* @code
* #include "mbed.h"
* #include "FXOS8700Q.h"
* 
* 
* int main(void) {
* 
* FXOS8700Q combo( A4, A5, FXOS8700Q_I2C_ADDRESS0);
* PwmOut rled(LED_RED);
* PwmOut gled(LED_GREEN);
* PwmOut bled(LED_BLUE);
* 
*     while (true) {       
*         rled1.0 - combo(acc.getAccX());
*         gled1.0 - combo(acc.getAccY());
*         bled1.0 - combo(acc.getAccZ());
*         wait(0.1);
*     }
* }
* @endcode
*/

class FXOS8700Q_acc : public MotionSensor
{
public:
  /**
  * FXOS8700Q constructor
  *
  * @param sda SDA pin
  * @param sdl SCL pin
  * @param addr addr of the I2C peripheral
  */
  
  FXOS8700Q_acc(PinName sda, PinName scl, int addr);

  /**
  * FXOS8700Q destructor
  */
  ~FXOS8700Q_acc();

    void enable(void);
    void disable(void);
    uint32_t sampleRate(uint32_t frequency);
    uint32_t whoAmI(void);
    uint32_t dataReady(void);
    void getX(int16_t * x);
    void getY(int16_t * y);
    void getZ(int16_t * z);
    void getX(float * x);
    void getY(float * y);
    void getZ(float * z);
    void getAxis(MotionSensorDataCounts &data);
    void getAxis(MotionSensorDataUnits &data);
  
  void readRegs(int addr, uint8_t * data, int len);
  
private:
  I2C m_i2c;
  int m_addr;

  void writeRegs(uint8_t * data, int len);
  int16_t getAccAxis(uint8_t addr);

};

class FXOS8700Q_mag : public MotionSensor
{
public:
  FXOS8700Q_mag(PinName sda, PinName scl, int addr);

  /**
  * FXOS8700Q destructor
  */
  ~FXOS8700Q_mag();

    void enable(void);
    void disable(void);
    uint32_t sampleRate(uint32_t fequency);
    uint32_t whoAmI(void);
    uint32_t dataReady(void);
    void getX(int16_t * x);
    void getY(int16_t * y);
    void getZ(int16_t * z);
    void getX(float * x);
    void getY(float * y);
    void getZ(float * z);
    void getAxis(MotionSensorDataCounts &data);
    void getAxis(MotionSensorDataUnits &data);
  
  void readRegs(int addr, uint8_t * data, int len);
  
private:
  I2C m_i2c;
  int m_addr;
  char sbuf[12];
  int sstatus;
  
  void writeRegs(uint8_t * data, int len);
  int16_t getAccAxis(uint8_t addr);

};

#endif
