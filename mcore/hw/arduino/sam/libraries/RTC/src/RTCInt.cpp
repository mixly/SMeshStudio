/*
  RTC library for Arduino M0/M0PRO.
  Copyright (c) 2015 Arduino SRL. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
 
#include "RTCInt.h"
//#include <Arduino.h>

static bool time_Mode = false;

/*******************************************************************************************
*Description: Function responsible of RTC initialization
*Input Parameters: bool timeRep this parameter can be TIME_H24 (24 hour mode) or TIME_H12 (12 hour mode)
*Return Parameter: None
*******************************************************************************************/
void RTCInt::begin(bool timeRep) 
{
  
  
  PM->APBAMASK.reg |= PM_APBAMASK_RTC; // turn on digital interface clock for RTC
  
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | (RTC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));
  while (GCLK->STATUS.bit.SYNCBUSY);
  
  GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_ID(4) | GCLK_GENCTRL_DIVSEL );
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(4); 
  GCLK->GENDIV.bit.DIV=4;
  while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
  
  

  RTCdisable();

  RTCreset();

  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_MODE_CLOCK; // set RTC operating mode
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_PRESCALER_DIV1024; // set prescaler to 1024 for MODE2
  RTC->MODE2.CTRL.bit.MATCHCLR = 0; // disable clear on match
  
  
  if (timeRep)
  {
    RTC->MODE2.CTRL.bit.CLKREP = 0; // 24h time representation
	time_Mode = true;
  }		
  else
  {  
    RTC->MODE2.CTRL.bit.CLKREP = 1; // 12h time representation
	time_Mode = false;
  }	

  RTC->MODE2.READREQ.reg &= ~RTC_READREQ_RCONT; // disable continuously mode

  
  while (RTCSync());
	RTCresetRemove();
  RTCenable();
  
}

/*
 * Get Time Functions
 */

/**********************************************************************************************************************
*Description: Function for getting hour, according to time representation mode this value can 
			  be in the range 0-24 (in case of TIME_H24) or in the range 0-12 (in case of TIME_H12)	
*Input Parameters: None
*Return Parameter: unsigned int (hour)
***********************************************************************************************************************/
 unsigned int RTCInt::getHour()
{
	unsigned int h=0;
	
	
	h=RTC->MODE2.CLOCK.bit.HOUR;
	while (RTCSync());
	
	if(time_Mode ==TIME_H12)
	{
		h = h & 0x0000000F;
	}
	
  return h;
}
 
 /**********************************************************************************************************************
*Description: Function for getting minute, this value varies in the range 0 - 59 
*Input Parameters: None
*Return Parameter: unsigned int (minute)
***********************************************************************************************************************/
unsigned int RTCInt::getMinute()
{
  unsigned int m=0;
  
  m=RTC->MODE2.CLOCK.bit.MINUTE;
  while (RTCSync());
 return m; 
}
 
 
 /**********************************************************************************************************************
*Description: Function for getting second, this value varies in the range 0 - 59 
*Input Parameters: None
*Return Parameter: unsigned int (second)
***********************************************************************************************************************/
 unsigned int RTCInt::getSecond()
{
  unsigned int s=0;
  
  s=RTC->MODE2.CLOCK.bit.SECOND;
  while (RTCSync());
 return s; 
}


/**********************************************************************************************************************
*Description: Function for getting time (hours, minutes and seconds). This function fills a structure called local_time
			  accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::getTime(void)
{
	unsigned int hour=0, h=0;
	
	local_time.hour = getHour();
	if(time_Mode == TIME_H12)
	{
		h=RTC->MODE2.CLOCK.bit.HOUR;
		while (RTCSync());
		local_time.Tmode = h & 0x10;
	}	
	local_time.minute = getMinute();
	local_time.second = getSecond();
}

unsigned char RTCInt::getMeridian(void)
{
	unsigned int h=0;
	unsigned char m=0;
	
	if(time_Mode == TIME_H12)
	{
		h=RTC->MODE2.CLOCK.bit.HOUR;
		while (RTCSync());
		m = h & 0x10;
		m >> 4;
	}
	else m=0;
return m;	
}


/*
 * Get Date Functions
 */

 
/**********************************************************************************************************************
*Description: Function for getting day 	
*Input Parameters: None
*Return Parameter: unsigned int day in the range 1 - 31
***********************************************************************************************************************/ 
unsigned int RTCInt::getDay()
{
  unsigned int d=0;
  
  d=RTC->MODE2.CLOCK.bit.DAY;
  while (RTCSync());
 return d; 
}

/**********************************************************************************************************************
*Description: Function for getting Month 	
*Input Parameters: None
*Return Parameter: unsigned int month in the range 1 - 12
***********************************************************************************************************************/
unsigned int RTCInt::getMonth()
{
  unsigned int month=0;
  
  month=RTC->MODE2.CLOCK.bit.MONTH;
  while (RTCSync());
 return month; 
}


/************************************************************************************************************************************
*Description: Function for getting year 	
*Input Parameters: None
*Return Parameter: unsigned int year in the range 0 - 63 (note: add offset to this value to obtain correct representation for year)
*************************************************************************************************************************************/
unsigned int RTCInt::getYear()
{
  unsigned int y=0;
  
  y=RTC->MODE2.CLOCK.bit.YEAR;
  while (RTCSync());
 return y; 
}


/**********************************************************************************************************************
*Description: Function for getting date (day, month and year). This function fills a structure called local_date
			  accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::getDate(void)
{
	local_date.day = getDay();
	local_date.month = getMonth();
	local_date.year = getYear();
}

/*
 * Set Time Functions
 */

 
/************************************************************************************************************************************
*Description: Function for setting hour 	
*Input Parameters: unsigned int hour (according to time representation the range can be 0 - 23 or 0 - 12)
*Return Parameter: None
*************************************************************************************************************************************/ 
void RTCInt::setHour(unsigned int hour, unsigned char meridian)
{
  if (time_Mode) {
    if((hour >= 0) && (hour <= 23))
	{	
		RTC->MODE2.CLOCK.bit.HOUR = hour;
		while (RTCSync());
	}
	else return;	
  } 
  else {
    if((hour >= 0) && (hour <=12)) 
	{	
		
		hour=hour | (meridian << 4);
		RTC->MODE2.CLOCK.bit.HOUR = hour;
		while (RTCSync());
	}
	else return;	
  }
  
}
 
 
 /************************************************************************************************************************************
*Description: Function for setting minute 	
*Input Parameters: unsigned int minute (range 0 - 59)
*Return Parameter: None
*************************************************************************************************************************************/ 
 void RTCInt::setMinute(unsigned int minute)
{
  if((minute >=0) && (minute <= 59))
  {		
	RTC->MODE2.CLOCK.bit.MINUTE = minute;
	while (RTCSync());
  }
  else return;	
}
 
 
 
/************************************************************************************************************************************
*Description: Function for setting second 	
*Input Parameters: unsigned int second (range 0 - 59)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setSecond(unsigned int second)
{
  if((second >=0) && (second <= 59))
  {		
	RTC->MODE2.CLOCK.bit.SECOND = second;
	while (RTCSync());
  }
  else return;	
}


/************************************************************************************************************************************
*Description: Function for setting time 	
*Input Parameters: unsigned int hour (range 0 - 23 or 0 - 12 according to time representation)
				   unsigned int minute (range 0 - 59)
				   unsigned int second (range 0 - 59)		
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setTime(unsigned int hour,unsigned char meridian, unsigned int minute, unsigned int second)
{
  setHour(hour,meridian);
  setMinute(minute);
  setSecond(second);  
}


/**********************************************************************************************************************
*Description: Function for setting time (hour, minute and second). This function sets time retrieving values from a 
			  local structure called local_time accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setTime(void)
{
	setHour(local_time.hour,local_time.Tmode);
    setMinute(local_time.minute);
    setSecond(local_time.second);  
}


/*
 * Set Date Functions
 */


/************************************************************************************************************************************
*Description: Function for setting day	
*Input Parameters: unsigned int day (range 1 - 31)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setDay(unsigned int day)
{
  if((day > 1) && (day <=31))
  {
  RTC->MODE2.CLOCK.bit.DAY = day;
  while (RTCSync());
  }
  else return;
}


/************************************************************************************************************************************
*Description: Function for setting month 	
*Input Parameters: unsigned int month (range 1 - 12)
*Return Parameter: None
*************************************************************************************************************************************/  
void RTCInt::setMonth(unsigned int month)
{
  if((month > 1) && (month <=12))
  {	  
  RTC->MODE2.CLOCK.bit.MONTH = month;
  while (RTCSync());
  }
  else return;
}


/************************************************************************************************************************************
*Description: Function for setting year 	
*Input Parameters: unsigned int year range (0 - 63) (note: add offset to this value to obtain correct representation for year)
*Return Parameter: None 
*************************************************************************************************************************************/
void RTCInt::setYear(unsigned int year)
{
  if((year >= 0) && (year <=63))
  {	  
  RTC->MODE2.CLOCK.bit.YEAR = year;
  while (RTCSync());
  }
  else return;
}


/**********************************************************************************************************************
*Description: Function for setting date (day, month and year).	
*Input Parameters: unsigned int day (range 1 - 31)
				   unsigned int month (range 1 - 12)
				   unsigned int year (range 0 - 63)		
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setDate(unsigned int day, unsigned int month, unsigned int year)
{
  setDay(day);
  setMonth(month);  
  setYear(year);
}


/**********************************************************************************************************************
*Description: Function for setting date (day, month and year). This function retrieves values from a local structure 
			  called local_date accessible in the class RTCInt.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::setDate(void)
{
	setDay(local_date.day);
	setMonth(local_date.month);  
    setYear(local_date.year);
}

/*
 * Set Alarm Functions
 */
 

 /**********************************************************************************************************************
*Description: Function for enabling alarm.	
*Input Parameters: unsigned int mode:
										 OFF                 alarm disabled
										 SEC                 alarm match on seconds
										 MMSS                alarm match on minutes and seconds
										 HHMMSS              alarm match on hours, minute and seconds
										 DDHHMMSS            alarm match on day, hours, minutes and seconds
										 MMDDHHMMSS          alarm match on month, day hours, minutes and seconds
										 YYMMDDHHMMSS        alarm match on year, month, day, hours, minutes and seconds	
				   unsigned int type:
										 ALARM_POLLED        alarm matching must be monitored from the code
										 ALARM_INTERRUPT     when alarm match occurs will be generated an interrupt
				   
				   voidFuncPtr callback: pointer to interrupt routine for alarm match (write NULL if you use ALARM_POLLED)		
*Return Parameter: None
***********************************************************************************************************************/
 void RTCInt::enableAlarm(unsigned int mode, unsigned int type, voidFuncPtr callback)
 {
	 
	 Alarm_Mode = mode;
	 RTC->MODE2.Mode2Alarm->MASK.reg = mode;
	 while (RTCSync());
	 
	 if(type == ALARM_POLLED)
	 {
		 return;
	 }	 
	 
	 else if(type == ALARM_INTERRUPT)
	 {
		 NVIC_DisableIRQ(RTC_IRQn); //disable any RTC interrupt
		 NVIC_ClearPendingIRQ(RTC_IRQn); //clear all pending interrupt for RTC
		 NVIC_SetPriority(RTC_IRQn,0);  //set RTC interrupt priority 
		 NVIC_EnableIRQ(RTC_IRQn);  //enable general interrupt for RTC
		 
		 RTC->MODE2.INTENSET.bit.ALARM0 = 1; //enable ALARM0 mathc
		 while (RTCSync());
		 
		 _callback = callback;
		 RTC->MODE2.INTFLAG.bit.ALARM0 = 1; //clear interrupt flag for RTC ALARM0 interrupt
		 
	 }		
 }
 
 
 /**********************************************************************************************************************
*Description: Function for setting alarm. This function retrieves values from two local structures 
			  called local_date and local_time accessible in the class RTCInt. According to match alarm type, values 
			  can be obtained from local_time, local_date or both structures.	
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
 void RTCInt::setAlarm(void)
 {
	 
   switch(RTCInt::Alarm_Mode)
   {
   case OFF :
      break;
   case SEC :
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
	  break;	
   case MMSS :
      RTC->MODE2.Mode2Alarm->ALARM.bit.MINUTE= local_time.minute;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
	  break;
   case HHMMSS :
      RTC->MODE2.Mode2Alarm->ALARM.bit.HOUR= local_time.hour;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.MINUTE= local_time.minute;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
      break;
   case DDHHMMSS :
      RTC->MODE2.Mode2Alarm->ALARM.bit.DAY= local_date.day;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.HOUR= local_time.hour;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.MINUTE= local_time.minute;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
      break;
   case MMDDHHMMSS :
      RTC->MODE2.Mode2Alarm->ALARM.bit.MONTH= local_date.month;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.DAY= local_date.day;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.HOUR= local_time.hour;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.MINUTE= local_time.minute;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
      break;
   case YYMMDDHHMMSS :
      RTC->MODE2.Mode2Alarm->ALARM.bit.YEAR= local_date.year;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.MONTH= local_date.month;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.DAY= local_date.day;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.HOUR= local_time.hour;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.MINUTE= local_time.minute;
	  while (RTCSync());
	  RTC->MODE2.Mode2Alarm->ALARM.bit.SECOND= local_time.second;
	  while (RTCSync());
      break;	  
   default :
      break;
   }
   
}

/**********************************************************************************************************************
*Description: Function for getting status of alarm match. This function is used when you adopt ALARM_POLLED mode 	
*Input Parameters: None
*Return Parameter: bool alarm match
***********************************************************************************************************************/
bool RTCInt::alarmMatch(void)
   {
	   return RTC->MODE2.INTFLAG.bit.ALARM0;
   }

/*
 * Private Utility Functions
 */



/**********************************************************************************************************************
*Description: Function for retrieving sync status of RTC 
*Input Parameters: None
*Return Parameter: bool RTC sync
***********************************************************************************************************************/
bool RTCInt::RTCSync()
{
  return (RTC->MODE2.STATUS.bit.SYNCBUSY);
}


/**********************************************************************************************************************
*Description: Function for disabling RTC
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::RTCdisable()
{
  RTC->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_ENABLE; // disable RTC
  while (RTCSync());
}


/**********************************************************************************************************************
*Description: Function for enabling RTC
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::RTCenable()
{
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_ENABLE; // enable RTC
  while (RTCSync());
}


/**********************************************************************************************************************
*Description: Function for resetting RTC. This function clears all RTC registers
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::RTCreset()
{
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_SWRST; // software reset
  while (RTCSync());
}


/**********************************************************************************************************************
*Description: Function for remove reset to RTC
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTCInt::RTCresetRemove()
{
  RTC->MODE2.CTRL.reg &= ~RTC_MODE2_CTRL_SWRST; // software reset remove
  while (RTCSync());
}



/**********************************************************************************************************************
*Description: RTC handler interrupt routine. This function checks flag ALARM0 and if is set calls routine 
			  pointed by _callback pointer then resets ALARM0 flag.
*Input Parameters: None
*Return Parameter: None
***********************************************************************************************************************/
void RTC_Handler(void)
{
	if(RTC->MODE2.INTFLAG.bit.ALARM0)
	{
		_callback(); //call interrupt routine
		RTC->MODE2.INTFLAG.bit.ALARM0 = 1; // clear ALARM0 flag
	}
	
	else return;	
}