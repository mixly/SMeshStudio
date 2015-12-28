/*********************************************************************************************************************************************************************************
* This sketch demonstrates how to use sleep function and wake-up MCU with RTC alarm interrupt.
  This sketch work only for M0 and M0PRO boards.
*********************************************************************************************************************************************************************************/  

#include <rest.h>
#include <RTCInt.h>

rest mcu_std;
RTCInt rtc;

unsigned int i=0;

void setup() 
{
  
  pinMode(30,OUTPUT);  //led RX
  Serial5.begin(9600);   //serial settings
  rtc.begin(TIME_H24);
  rtc.setTime(12,48,0,0);
  rtc.setDate(22,10,15);
  rtc.enableAlarm(SEC,ALARM_INTERRUPT,rest_alarm_int);
  rtc.local_time.hour=12;
  rtc.local_time.minute=48;
  rtc.local_time.second=30;
  mcu_std.begin(WAKE_RTC_ALARM);  //standby setup for external interrupts
  rtc.setAlarm();
  SerialUSB.println("START");

}

void loop() 
{
  for(i=0; i<20; i++)
  {
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    delay(200);
  }  
  
  digitalWrite(30,LOW);
  mcu_std.standby();  //now mcu go to standby

}


void rest_alarm_int(void)  //interrupt routine 
{
  //PORT->Group[1].OUTSET.reg = 0x1u<<3;
  digitalWrite(30,HIGH);
  
  Serial5.println("HELLO");
} 
