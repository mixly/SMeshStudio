#include "LowPower.h"
#include <MxTimer2.h>

#define led_pin 27

void flash()
{
static boolean output = HIGH;

  digitalWrite(led_pin, output);
  output = !output;
  LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF,TIMER2_ON);
}

void setup()
{



	pinMode(led_pin,OUTPUT);
	digitalWrite(led_pin,HIGH);
	pinMode(28,OUTPUT);
	pinMode(29,OUTPUT);
	MxTimer2::set(64, 128, flash); // call every 500 1ms "ticks"
	MxTimer2::start();

}

void loop()
{
	//LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF,TIMER2_ON);
}
