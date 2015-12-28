/**********************************************************************************************************************************************************
This sketch gives a simple demonstration of how to use RTC library.
The code sets date and time using internal structure and then print on serial date and time. Time representation is 12 hour mode
**********************************************************************************************************************************************************/



#include <RTCInt.h>

RTCInt rtc;  //create an RTCInt type object


void setup() 
{
  Serial.begin(9600);
  rtc.begin(TIME_H12); //init RTC in 12 hour mode
  
  //filling internal structure for time
  rtc.local_time.hour = 10;          //hour
  rtc.local_time.minute = 44;       //minute
  rtc.local_time.second = 0;        //second
  rtc.local_time.Tmode = ANTI_MERIDIAN;  
  
  //filling internal structure for date
  rtc.local_date.day = 13;        //day
  rtc.local_date.month = 8;       //month
  rtc.local_date.year = 15;       //year
  
  rtc.setTime();  //setting time
  rtc.setDate();  //setting date
  
  
  
  

}

void loop() 
{
 rtc.getDate();      //getting date in local structure (local_date)
 rtc.getTime();      //getting time in local structure(local_time)
 
 //printing date in format YYYY/MM/DD
 Serial.print(rtc.local_date.year+2000); // year
 Serial.print('/');
 Serial.print(rtc.local_date.month);    // month
 Serial.print('/');
 Serial.print(rtc.local_date.day);      // day
 Serial.print(' ');
 
 //printing time
 Serial.print(rtc.local_time.hour);    //hour
 Serial.print(':');
 Serial.print(rtc.local_time.minute);  //minute
 Serial.print(':');
 Serial.print(rtc.local_time.second);  //second
 Serial.print (' ');
 if(rtc.local_time.Tmode == ANTI_MERIDIAN) Serial.println("AM"); // print AM or PM
 else Serial.println("PM");
 
 delay(1000);
}
