

#include <string.h>
#include "contiki.h"
#include "wait_api.h"
#include "ds2401.h"
#include "gpio_api.h"
unsigned char ds2401_id[8];
gpio_t owpin;

void writeBit(int bit) {
	gpio_dir(&owpin, PIN_OUTPUT);;
	if (bit==1)
	{
		gpio_write(&owpin, 0);
		wait_us(10);
		gpio_write(&owpin, 1);
		wait_us(55);
	}
	else
	{
		gpio_write(&owpin, 0);
		wait_us(65);
		gpio_write(&owpin, 1);
		wait_us(5);

	}
}
int readBit() {
	int pin;
	gpio_dir(&owpin, PIN_OUTPUT);
	gpio_write(&owpin, 0);
	wait_us(3);
	gpio_write(&owpin, 1);
	wait_us(10);
	gpio_dir(&owpin, PIN_INPUT);
	pin = gpio_read(&owpin);
	gpio_dir(&owpin, PIN_OUTPUT);
	wait_us(53);
	return pin;
}
/*---------------------------------------------------------------------------*/
static uint8_t
reset(void)
{
	int r;
	gpio_init(&owpin,P1_29);
	gpio_mode(&owpin,PullUp);
	gpio_dir(&owpin, PIN_OUTPUT);
	gpio_write(&owpin, 0);
	wait_us(540);
	gpio_write(&owpin, 1);
	gpio_dir(&owpin, PIN_INPUT);
	wait_us(65);
	r = gpio_read(&owpin);;
	gpio_dir(&owpin, PIN_OUTPUT);
	wait_us(540);
	return r;
}
/*---------------------------------------------------------------------------*/
static void
write_byte(uint8_t byte)
{
	int b;
	int i;
	for (i=0; i<=7; i++) {
		b = byte & 0x01;
		writeBit(b);
		byte = byte >> 1;
	}
}
/*---------------------------------------------------------------------------*/
static unsigned
read_byte(void)
{
	unsigned char i;
	unsigned char wert = 0;
	for (i=0; i<8; i++) {
		if (readBit()) wert |=0x01 << i;
	}
	return(wert);
}

/*---------------------------------------------------------------------------*/
/* Polynomial ^8 + ^5 + ^4 + 1 */
static unsigned
crc8_add(unsigned acc, unsigned byte)
{
	int i;
	acc ^= byte;
	for (i = 0; i < 8; i++)
		if (acc & 1)
			acc = (acc >> 1) ^ 0x8c;
		else
			acc >>= 1;
	return acc;
}
/*---------------------------------------------------------------------------*/
int
ds2401_init()
{
	int i;
	unsigned family, crc, acc;


	if (reset() == 0) {
		write_byte(0x33);    /* Read ROM command. */
		family = read_byte();
		for (i = 7; i >= 2; i--) {
			ds2401_id[i] = read_byte();
		}
		crc = read_byte();
		if(family != 0x01) {
			goto fail;
		}
		acc = crc8_add(0x0, family);
		for (i = 7; i >= 2; i--) {
			acc = crc8_add(acc, ds2401_id[i]);
		}
		if (acc == crc) {
			/* 00:1A:4C    OUI for Crossbow Technology, Inc. */
			ds2401_id[0] = 0x00;
			ds2401_id[1] = 0x1A;
			ds2401_id[2] = 0x4C;
			return 1;	/* Success! */
		}
	}
	fail:
	memset(ds2401_id, 0x0, sizeof(ds2401_id));
	return 0;  /* Fail! */
}
/*---------------------------------------------------------------------------*/
