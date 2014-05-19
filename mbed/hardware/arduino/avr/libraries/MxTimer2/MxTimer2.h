#ifndef MxTimer2_h
#define MxTimer2_h

#ifdef __AVR__
#include <avr/interrupt.h>
#else
#error MxTimer2 library only works on AVR architecture
#endif
typedef enum{
	Prescale_1=1,
	Prescale_8=8,
	Prescale_32=32,
	Prescale_64=64,
	Prescale_128=128,
	Prescale_256=256,
	Prescale_1024=1024,
} Prescale_type_t;

namespace MxTimer2 {
	extern unsigned long time_units;
	extern void (*func)();
	extern volatile unsigned long count;
	extern volatile char overflowing;
	void set(unsigned long units,void (*f)());
	void set(unsigned long units, unsigned int resolution, void (*f)());
	void start();
	void stop();
	void _overflow();
	void sleepms(unsigned char howlong);
}
#endif
