/*
  MxTimer2.h - Using timer2 with a configurable resolution user 32.768k
  Fredqian <fuqian@smeshlink.com>
  Changed from FlexTimer2

 */
 
#include <MxTimer2.h>

unsigned long MxTimer2::time_units;
void (*MxTimer2::func)();
volatile unsigned long MxTimer2::count;
volatile char MxTimer2::overflowing;
//设置分辨率，resolution表示1秒中断多少次，units表示在resolution分辨率下中断次数，f表示中断了//unit后需要执行的函数
void MxTimer2::set(unsigned long units,  void (*f)()) {
	if (units == 0)
		time_units = 1;
	else
		time_units = units;
	func = f;
#if  defined (__AVR_ATmega1284P__) || defined (__AVR_AT90USB1287__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega128RFA1__) || defined(__AVR_ATmega256RFR2__)
	cli();
	ASSR |= (1 << AS2);
	TCCR2A =(1<<WGM21);

	OCR2A = 0;
	TCCR2B =3;//prescale=32

	TCNT2 = 0;
	while(ASSR & (1 << TCN2UB));

	SMCR |= (1 <<  SE);
	TIMSK2 &= ~_BV (OCIE2A);
	sei();
#elif defined (__AVR_ATmega8__)
	cli();
	ASSR |= (1 << AS2);
	TCCR2 =(1<<WGM21);

	OCR2 = 0;
	TCCR2 =3;//prescale=32

	TCNT2 = 0;
	while(ASSR & (1 << TCN2UB));

	TIMSK &= ~_BV (OCIE2);
	sei();
/*
	TIMSK &= ~(1<<TOIE2);
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	TIMSK &= ~(1<<OCIE2);
	ASSR &= ~(1<<AS2);

	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2 |= ((1<<CS22) && (1<<CS20));
		TCCR2 &= ~(1<<CS21);
		prescaler = 128.0;
	}
	*/
#else
#error Unsupported CPU type
#endif
}
//设置分辨率，resolution表示1秒中断多少次，units表示在resolution分辨率下中断次数，f表示中断了//unit后需要执行的函数
void MxTimer2::set(unsigned long units, unsigned int resolution, void (*f)()) {
	if (units == 0)
		time_units = 1;
	else
		time_units = units;
	func = f;
#if defined (__AVR_ATmega1284P__) || (__AVR_AT90USB1287__) || (__AVR_ATmega1281__) || defined (__AVR_ATmega128RFA1__)
	cli();
	ASSR |= (1 << AS2);
	TCCR2A =(1<<WGM21);

	OCR2A = 32768/32/resolution - 1;
	TCCR2B =3;//prescale=32

	TCNT2 = 0;
	while(ASSR & (1 << TCN2UB));

	SMCR |= (1 <<  SE);
	TIMSK2 &= ~_BV (OCIE2A);
	sei();
#else
#error Unsupported CPU type
#endif
}


void MxTimer2::start() {
	count = 0;
	overflowing = 0;
#if  defined (__AVR_ATmega1284P__) || defined(__AVR_AT90USB1287__) || defined(__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega128RFA1__)  || defined(__AVR_ATmega256RFR2__)
	TCNT2 = 0;
	TIMSK2 = _BV (OCIE2A); //启用中断
#elif defined (__AVR_ATmega8__)
	TCNT2 = 0;
	TIMSK = _BV (OCIE2); //启用中断
#endif
}
void MxTimer2::stop() {
#if defined (__AVR_ATmega1284P__) || defined(__AVR_AT90USB1287__) || defined (__AVR_ATmega2560__) || defined(__AVR_ATmega1281__) || defined (__AVR_ATmega128RFA1__)
	TIMSK2 &= ~_BV (OCIE2A); //禁止中断
#elif defined (__AVR_ATmega8__)
	TCNT2 = 0;
	TIMSK &= ~_BV (OCIE2); //禁止中断
#endif
}
//中断实际处理函数
void MxTimer2::_overflow() {
	count += 1;
	if (count >= time_units && !overflowing) {
		overflowing = 1;
		count = count - time_units;
		(*func)();
		overflowing = 0;
	}
}

/*---------------------------------------------------------------------------*/

void MxTimer2::sleepms(unsigned char howlong)
{
	/* Deep Sleep for howlong rtimer ticks. This will stop all timers except
	 * for TIMER2 which can be clocked using an external crystal.
	 * Unfortunately this is an 8 bit timer; a lower prescaler gives higher
	 * precision but smaller maximum sleep time.
	 * Here a maximum 128msec (contikimac 8Hz channel check sleep) is assumed.
	 * The rtimer and system clocks are adjusted to reflect the sleep time.
	 */
#include <avr/sleep.h>



	/* Save TIMER2 configuration for clock.c is using it */
#if defined (__AVR_ATmega1284P__) || defined(__AVR_AT90USB1287__) || defined (__AVR_ATmega2560__) || defined(__AVR_ATmega1281__) || defined (__AVR_ATmega128RFA1__)
	uint8_t savedTCNT2=TCNT2, savedTCCR2A=TCCR2A, savedTCCR2B = TCCR2B, savedOCR2A = OCR2A;
	cli();
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	TCCR2A = _BV(WGM21);
	TCCR2B =((0<<CS22)|(1<<CS21)|(1<<CS20));

	/* Set TIMER2 clock asynchronus from external source, CTC mode */
	ASSR |= (1 << AS2);
	//TCCR2A =(1<<WGM21);
	// Prescale by  32 - 1024 ticks/sec, 250 msec max sleep
	TCCR2A = _BV(WGM21);
	TCCR2B =((0<<CS22)|(1<<CS21)|(1<<CS20));
	OCR2A = howlong-1;

	/* Reset timer count, wait for the write (which assures TCCR2x and OCR2A are finished) */
	TCNT2 = 0;
	while(ASSR & (1 << TCN2UB));

	/* Enable TIMER2 output compare interrupt, sleep mode and sleep */
	TIMSK2 |= (1 << OCIE2A);
	SMCR |= (1 <<  SE);
	sei();
	/* Ensure no activity is suspending the low power mode
	   before going to sleep. (edited by smeshlink) */
	if (OCR2A) sleep_mode();
	//...zzZZZzz...Ding!//

	/* Disable sleep mode after wakeup, so random code cant trigger sleep */
	SMCR  &= ~(1 << SE);
	/* Restore clock.c configuration */
	cli();
	count += howlong;
	TCCR2A = savedTCCR2A;
	TCCR2B = savedTCCR2B;
	OCR2A  = savedOCR2A;
	TCNT2  = savedTCNT2;
	sei();
#elif defined (__AVR_ATmega8__)
	uint8_t savedTCNT2=TCNT2, savedTCCR2A=TCCR2, savedTCCR2B = TCCR2, savedOCR2A = OCR2;
	cli();
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		TCCR2 = _BV(WGM21);
		TCCR2 =((0<<CS22)|(1<<CS21)|(1<<CS20));

		/* Set TIMER2 clock asynchronus from external source, CTC mode */
		ASSR |= (1 << AS2);
		//TCCR2A =(1<<WGM21);
		// Prescale by  32 - 1024 ticks/sec, 250 msec max sleep
		TCCR2 = _BV(WGM21);
		TCCR2 =((0<<CS22)|(1<<CS21)|(1<<CS20));
		OCR2 = howlong-1;

		/* Reset timer count, wait for the write (which assures TCCR2x and OCR2A are finished) */
		TCNT2 = 0;
		while(ASSR & (1 << TCN2UB));

		/* Enable TIMER2 output compare interrupt, sleep mode and sleep */
		TIMSK |= (1 << OCIE2);

		sei();
		/* Ensure no activity is suspending the low power mode
		   before going to sleep. (edited by smeshlink) */
		if (OCR2) sleep_mode();
		//...zzZZZzz...Ding!//


		/* Restore clock.c configuration */
		cli();
		count += howlong;
		TCCR2 = savedTCCR2A;
		TCCR2 = savedTCCR2B;
		OCR2  = savedOCR2A;
		TCNT2  = savedTCNT2;
		sei();
#endif

}

ISR(TIMER2_COMPA_vect) {
	MxTimer2::_overflow();
}
