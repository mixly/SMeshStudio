/*********************************************************************************************************************************************************************************
* This sketch demonstrates how sleep function using external interrupt to wake up the MCU.
  This sketch work only for M0 or M0PRO boards.
*********************************************************************************************************************************************************************************/  



#include <rest.h>

rest mcu_std;

unsigned int i=0;

void setup() 
{
  
  //SerialUSB.begin(9600);   //serial settings
  mcu_std.begin(WAKE_EXT_INTERRUPT,8,prova);  //standby setup for external interrupts
  

}

void loop() 
{
  for(i=0; i<20; i++)
  {
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    delay(500);
  }  
  
  mcu_std.standby();  //now mcu go to standby

}


void prova(void)  //interrupt routine (isn't necessary to execute any tasks in this routine
{
  
} 
