
#include <softI2C.h>
#include <wiring_private.h>
#include <util/delay.h>

softI2C::softI2C(int dat,int clk,int addr) {
	_dataP = dat;
	_clockP = clk;
	_address = addr;

}
/* ================  Public methods ================ */
void softI2C::begin(){
	i2c_init();
}
void softI2C::writeReg(unsigned char RegNum, unsigned char Value, int type){
	i2c_start();
	i2c_write(_address<<1);
	i2c_write(RegNum);
	i2c_write(Value);
	i2c_end();
}
unsigned char softI2C::readReg(unsigned char RegNum, unsigned char type){
	i2c_start();
	i2c_write(_address<<1);
	i2c_write(RegNum);
	i2c_start();
	i2c_write((_address<<1) + 1);
	unsigned char tmp=i2c_read();
	i2c_nack();
	i2c_end();
	return tmp;
}

/* ================  Private methods ================ */
// hardware pin functions

void softI2C::pulse_clock() {
	_delay_us(100);
	
	_delay_us(100);
	digitalWrite(_clockP,LOW);
}

unsigned char softI2C::read_bit() {
	unsigned char i;
	pinMode(_dataP,INPUT);
	_delay_us(100);
	digitalWrite(_clockP,HIGH);
	_delay_us(100);
	i = (unsigned char) digitalRead(_dataP);
	digitalWrite(_clockP,LOW);
	return i;
}

unsigned char softI2C::i2c_read() {
	unsigned char data = 0;
	unsigned char i = 0;
	for (i = 0; i < 8; i++) {
		data = (data << 1) & 0xfe;
		if (read_bit() == 1) {
			data |= 0x1;
		}
	}
	return data;
}

unsigned char softI2C::i2c_write(unsigned char c) {
	unsigned char i;
	pinMode(_dataP,OUTPUT);
	for (i = 0; i < 8; i++) {
		if (c & 0x80) {
			digitalWrite(_dataP,HIGH);
		} else {
			digitalWrite(_dataP,LOW);
		}
		pulse_clock();
		c = c << 1;
	}
	i = read_bit();
	return i == 0;
}
void softI2C::i2c_init() {
	digitalWrite(_clockP,HIGH);
	digitalWrite(_dataP,HIGH);
	pinMode(_clockP,OUTPUT);
	pinMode(_dataP,OUTPUT);
}
void softI2C::i2c_start() {
	digitalWrite(_dataP,HIGH);
	digitalWrite(_clockP,HIGH);
	pinMode(_dataP,OUTPUT);
	_delay_us(100);
	digitalWrite(_dataP,LOW);
	_delay_us(100);
	digitalWrite(_clockP,LOW);
}

void softI2C::i2c_ack() {
	pinMode(_dataP,OUTPUT);
	digitalWrite(_dataP,LOW);
	pulse_clock();
}

void softI2C::i2c_nack() {
	pinMode(_dataP,OUTPUT);
	digitalWrite(_dataP,HIGH);
	pulse_clock();
}

void softI2C::i2c_sendack(char ack) {
	if (!ack)
		i2c_ack();
	else
		i2c_nack();
}

void softI2C::i2c_end() {
	pinMode(_dataP,OUTPUT);
	digitalWrite(_dataP,LOW);
	_delay_us(100);
	digitalWrite(_clockP,HIGH);
	_delay_us(100);
	digitalWrite(_dataP,HIGH);
}
