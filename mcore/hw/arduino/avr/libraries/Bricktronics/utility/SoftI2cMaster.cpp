#include "SoftI2cMaster.h"

//------------------------------------------------------------------------------
// WARNING don't change anything unless you verify the change with a scope
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// init pins and set bus high
void SoftI2cMaster::init(uint8_t sclPin, uint8_t sdaPin){
  sclPin_ = sclPin;
  sdaPin_ = sdaPin;
  digitalWrite(sclPin_, LOW);
  digitalWrite(sdaPin_, LOW);
  pinMode(sclPin_,INPUT);
  pinMode(sdaPin_,INPUT);
}

//------------------------------------------------------------------------------
// read a byte and send Ack if last is false else Nak to terminate read
uint8_t SoftI2cMaster::read(uint8_t last){
  uint8_t b = 0;
  pinMode(sdaPin_, INPUT);
  // read byte
  for (uint8_t i = 0; i < 8; i++) {
  // don't change this loop unless you verify the change with a scope
  b <<= 1;
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,INPUT);
  if (digitalRead(sdaPin_)) b |= 1;
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,OUTPUT);
  }
  // send Ack or Nak
  pinMode(sdaPin_, OUTPUT);
  if (last)  pinMode(sdaPin_, INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,OUTPUT);
  pinMode(sdaPin_,INPUT);
  return b;
}
//------------------------------------------------------------------------------
// send new address and read/write with stop
uint8_t SoftI2cMaster::restart(uint8_t addressRW){
  delayMicroseconds(I2C_DELAY_USEC);
  stop();
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,OUTPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  start(addressRW);
  delayMicroseconds(I2C_DELAY_USEC);
  return true;
}
//------------------------------------------------------------------------------
// issue a start condition for i2c address with read/write bit
uint8_t SoftI2cMaster::start(uint8_t addressRW){
  pinMode(sdaPin_,OUTPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,OUTPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  return write(addressRW);
}
//------------------------------------------------------------------------------
// issue a stop condition
void SoftI2cMaster::stop(void){
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sclPin_,INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sdaPin_,INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
}
//------------------------------------------------------------------------------
// write byte and return true for Ack or false for Nak
uint8_t SoftI2cMaster::write(uint8_t b){
  for (uint8_t m = 0X80; m != 0; m >>= 1) {
    if (m & b) 
    {
	pinMode(sdaPin_, INPUT);
    }
    else
    {
	 pinMode(sdaPin_, OUTPUT);
    }
    // don't change this loop unless you verify the change with a scope
    pinMode(sclPin_,INPUT);
    delayMicroseconds(I2C_DELAY_USEC);
    pinMode(sclPin_,OUTPUT);
    delayMicroseconds(I2C_DELAY_USEC);
  }
  pinMode(sdaPin_,INPUT);
  pinMode(sdaPin_, INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  //extra clock for writes, for the LEGO firmware bug.
  pinMode(sclPin_,INPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  b = digitalRead(sdaPin_);
  pinMode(sclPin_,OUTPUT);
  delayMicroseconds(I2C_DELAY_USEC);
  pinMode(sdaPin_, OUTPUT);
  if ( b != 0 ) stop ();
  return b == 0;
}

