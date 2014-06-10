#include "contiki.h"
#include "Arduino.h"

PROCESS(leds_blink_process, "Hello world process");
PROCESS_THREAD(leds_blink_process, ev, data)
{
  static struct etimer timer;
  /* Any process must start with this. */
  PROCESS_BEGIN();

  while(1) {
    /* Set the etimer every time. */
    etimer_set(&timer, CLOCK_CONF_SECOND*1);
    /* And wait until the specific event. */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	  //push_mcu_need_alive_state();
    digitalWrite(28,!digitalRead(28));

    Serial.print(millis(),DEC);
    Serial.print("  hello\n");
    Serial.flush();
    etimer_reset(&timer);
    //pop_mcu_need_alive_state();
  }
  /* Any process must end with this, even if it is never reached. */
  PROCESS_END();
}
PROCESS(print_blink_process, "Hello world process2");
PROCESS_THREAD(print_blink_process, ev, data)
{
  static struct etimer timer;
  /* Any process must start with this. */
  PROCESS_BEGIN();

  while(1) {
    /* Set the etimer every time. */
    etimer_set(&timer, CLOCK_CONF_SECOND*1);
    /* And wait until the specific event. */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	  //push_mcu_need_alive_state();
    digitalWrite(29,!digitalRead(29));

    Serial.print(millis(),DEC);
    Serial.print("  world\n");
    Serial.flush();
    etimer_reset(&timer);
    //pop_mcu_need_alive_state();
  }

  /* Any process must end with this, even if it is never reached. */
  PROCESS_END();
}
//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	initialize();
	pinMode(28,OUTPUT);
	pinMode(29,OUTPUT);
	TRXPR=1<<SLPTR;
	digitalWrite(28,HIGH);
	digitalWrite(29,HIGH);
	//_delay_ms(1000);
	//digitalWrite(29,LOW);
	Serial.begin(38400);
	process_start(&leds_blink_process, NULL);
	process_start(&print_blink_process, NULL);
}

// The loop function is called in an endless loop
void loop()
{


	clock_arch_sleepms(250);

	/*
	digitalWrite(28,LOW);
	clock_arch_sleepms(200);
	digitalWrite(28,HIGH);
	clock_arch_sleepms(200);
*/
}