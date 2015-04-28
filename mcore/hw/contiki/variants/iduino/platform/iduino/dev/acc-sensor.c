
#include "dev/acc-sensor.h"
static int enabled=0;
unsigned int
readReg(char i2caddr,char reg)
{
	uint8_t buf[1];
	buf[0]=reg;
	twi_writeTo(i2caddr, buf, 1, 1,1);
	twi_readFrom(i2caddr, buf, 1, 1);
	return buf[0];
}
void
writeReg(char i2caddr,char reg,char data)
{
	uint8_t buf[2];
	buf[0]=reg;
	buf[1]=data;
	twi_writeTo(i2caddr, buf, 2, 1,1);
}

static int
value(int type)
{
	uint8_t reg_addr;
	switch(type) {
	case ACC_X_AXIS:
		reg_addr = OUTX_L;
		break;
	case ACC_Y_AXIS:
		reg_addr = OUTY_L;
		break;
	case ACC_Z_AXIS:
		reg_addr = OUTZ_L;
		break;
	default:
		return 0;
	}
	int l=readReg(LIS3DH_ADDR,reg_addr);
	int h=readReg(LIS3DH_ADDR,reg_addr+1);
	return h<<8|l;
}
static int
configure(int type, int value)
{
	switch(type) {

	case SENSORS_HW_INIT:
		return 1;
	case SENSORS_ACTIVE:
		if(value){
			twi_init();
			writeReg(LIS3DH_ADDR,CTRL_REG1,0x27);
			enabled=1;
			return 1;
		}
		else
			enabled=0;

	}
	return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
	return enabled;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(acc_sensor, ACC_SENSOR, value, configure, status);


/** @} */
