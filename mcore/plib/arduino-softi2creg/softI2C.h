/**
 * softI2C Library
 *
 */
#ifndef softI2C_h
#define softI2C_h

class softI2C {
public:
	void begin();
	void writeReg(unsigned char RegNum, unsigned char Value, int type);
	unsigned char readReg(unsigned char RegNum, unsigned char type);
	softI2C(int dat,int clk, int addr);
private:
	int _dataP;
	int _clockP;
	int _address;
	void pulse_clock();
	unsigned char read_bit();
	unsigned char i2c_read();
	unsigned char i2c_write(unsigned char c);
	void i2c_init();
	void i2c_start();
	void i2c_ack();
	void i2c_nack();
	void i2c_sendack(char ack);
	void i2c_end();
};

#endif
