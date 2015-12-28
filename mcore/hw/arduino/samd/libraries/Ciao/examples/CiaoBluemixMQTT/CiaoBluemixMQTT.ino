/*
  Arduino Ciao example
  
 This sketch uses Ciao mqtt connector. Sketch sends via MQTT brightness and temperature information that will be shown graphically in the blueMix IBM system.
 Upload your sketch an than connect to the webpage:  
 https://quickstart.internetofthings.ibmcloud.com/#/device/BlueMixTest902345/sensor/
 
  NOTE: be sure to activate and configure mqtt connector on Linino OS.
       http://labs.arduino.org/Ciao

 created September 2015
 by andrea[at]arduino[dot]org
   
 */

#include <Ciao.h>

int ctrl=0;
void setup()
{
  Ciao.begin();  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
  pinMode(9,OUTPUT);
}
 
void loop()                     // run over and over again
{
   //getting the voltage reading from the temperature sensor
   int readingTemp = analogRead(A0);   
   // converting readingTemp to voltage
   float voltage = readingTemp * 4.56;
   voltage /= 1024; 
   // now print out the temperature
   float temperatureC = (voltage - 0.5) * 100 ; 
   int readingLum = analogRead(A2);

   analogWrite(9,map(readingLum,0,1023,0,255)); 
   
   if (ctrl>=10){
     Ciao.write("mqtt","iot-2/evt/status/fmt/json","{\"d\": {\"temperature\":"+String(temperatureC)+",\"luminosity\":"+String(readingLum)+"}}");
     ctrl=0;
   }
   ctrl++;
   delay(100);
 
}