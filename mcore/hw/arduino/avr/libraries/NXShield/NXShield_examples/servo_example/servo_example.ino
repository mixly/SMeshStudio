#include <Wire.h>
#include <NXShield.h>
//#include <NXShieldAGS.h>
#include <Servo.h>
#define GLOBAL_DELAY 50


Servo           myservo2;       // create servo object to control a servo 
Servo           myservo3;       // create servo object to control a servo 
Servo           myservo4;       // create servo object to control a servo 
Servo           myservo5;       // create servo object to control a servo 
Servo           myservo6;       // create servo object to control a servo 
Servo           myservo7;       // create servo object to control a servo 
Servo           myservo8;       // create servo object to control a servo
Servo           myservo9;       // create servo object to control a servo
Servo           myservo10;      // create servo object to control a servo
Servo           myservo11;      // create servo object to control a servo
Servo           myservo12;      // create servo object to control a servo
Servo           myservo13;      // create servo object to control a servo

int             pos = 0;        // variable to store the servo position 

void setup()
{
     
  Serial.begin(115200);  
     Serial.println ("Running Servos");
     
       #ifdef MODEL_NXSHIELD_D

    myservo3.attach(3);
    myservo5.attach(5);
    myservo6.attach(6);
    myservo9.attach(9);
    myservo10.attach(10);
    myservo11.attach(11);

#else

    myservo2.attach(2);
    myservo3.attach(3);
    myservo4.attach(4);
    myservo5.attach(5);
    myservo6.attach(6);
    myservo7.attach(7);
    myservo8.attach(8);
    myservo9.attach(9);
    myservo10.attach(10);
    myservo11.attach(11);
    myservo12.attach(12);
    myservo13.attach(13);

#endif
}   
void loop()
{
 
  pos = 255;
  myservo2.write(pos);    // tell servo to go to 'pos'
  myservo3.write(pos);    // tell servo to go to 'pos'
  myservo4.write(pos);    // tell servo to go to 'pos'
  myservo5.write(pos);    // tell servo to go to 'pos'
  myservo6.write(pos);    // tell servo to go to 'pos'
  myservo7.write(pos);    // tell servo to go to 'pos'
  myservo8.write(pos);    // tell servo to go to 'pos'
  myservo9.write(pos);    // tell servo to go to 'pos'
  myservo10.write(pos);   // tell servo to go to 'pos'
  myservo11.write(pos);   // tell servo to go to 'pos'
  myservo12.write(pos);   // tell servo to go to 'pos'
  myservo13.write(pos);   // tell servo to go to 'pos'
  delay(500);

  pos = 0;
  myservo2.write(pos);    // tell servo to go to 'pos'
  myservo3.write(pos);    // tell servo to go to 'pos'
  myservo4.write(pos);    // tell servo to go to 'pos'
  myservo5.write(pos);    // tell servo to go to 'pos'
  myservo6.write(pos);    // tell servo to go to 'pos'
  myservo7.write(pos);    // tell servo to go to 'pos'
  myservo8.write(pos);    // tell servo to go to 'pos'
  myservo9.write(pos);    // tell servo to go to 'pos'
  myservo10.write(pos);   // tell servo to go to 'pos'
  myservo11.write(pos);   // tell servo to go to 'pos'
  myservo12.write(pos);   // tell servo to go to 'pos'
  myservo13.write(pos);   // tell servo to go to 'pos'
  delay(500);

}
