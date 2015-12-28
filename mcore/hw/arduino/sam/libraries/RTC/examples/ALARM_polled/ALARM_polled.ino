/*********************************************************************************************************************************************************************************
* This sketch demonstrates how configure alarm in polled mode.
  In this mode you hav to check continuously ALARM0 flag in the main program code.
*********************************************************************************************************************************************************************************/  


#include <RTCInt.h>

RTCInt rtc;

void setup() 
{
  Serial.begin(9600);       //serial communication initializing
  pinMode(13,OUTPUT);
  rtc.begin(TIME_H24);      //RTC initializing with 24 hour representation mode
  rtc.setTime(17,0,5,0);    //setting time (hour minute and second)
  rtc.setDate(13,8,15);     //setting date
  rtc.enableAlarm(SEC,ALARM_POLLED,NULL); //enabling alarm in polled mode and match on second
  rtc.local_time.hour=17;
  rtc.local_time.minute=5;
  rtc.local_time.second=10;  //setting second to match
  rtc.setAlarm();  //write second in alarm register
}

void loop()
{

   digitalWrite(13,HIGH);
   delay(100);
   digitalWrite(13,LOW);
   delay(400);
   
  if(rtc.alarmMatch())        //when match occurs led on pin 13 blinks ten times
  {
    Serial.println("Alarm match!");
    for(int i=0; i < 10; i++)
    {
      digitalWrite(13,HIGH);
      delay(200);
      digitalWrite(13,LOW);
      delay(800);
    }
    RTC->MODE2.INTFLAG.bit.ALARM0=1; //clearing alarm0 flag
  } 
   
}
