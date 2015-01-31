/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/*
For beginners tutorial (and user guide) visit:
http://www.openelectrons.com/docs/viewdoc/1
*/

#include <Wire.h>
#include <NXShield.h>
#define GLOBAL_DELAY 50

/**
  This is a simple example of blinking
	the NXShield LED with multiple colors.
*/
NXShield nxshield;

void setup()
{
  Serial.begin(115200);

  nxshield.init(SH_SoftwareI2C);

  Serial.println ("Watch the LED on NXShield ...");
}

void loop()
{
      multi_color_blink();
}

void multi_color_blink(){
 for (int i=8; i>=0; i--){
   if (i<=4) nxshield.ledSetRGB(i,4-i,0);
   else    nxshield.ledSetRGB(i,0,0);
   delay(GLOBAL_DELAY);
 }
 
 for (int i=5; i<=8; i++){
   nxshield.ledSetRGB(0,i,0);
   delay(GLOBAL_DELAY/2);
 }

 for (int i=8; i>=0; i--){
   if (i<=4) nxshield.ledSetRGB(0,i,4-i);
   else    nxshield.ledSetRGB(0,i,0);
   delay(GLOBAL_DELAY);
 }

 for (int i=5; i<=8; i++){
   nxshield.ledSetRGB(0,0,i);
   delay(GLOBAL_DELAY/2);
 }

 for (int i=8; i>=0; i--){
   if (i<=4) nxshield.ledSetRGB(4-i,0,i);
   else    nxshield.ledSetRGB(0,0,i);
   delay(GLOBAL_DELAY);
 }

 for (int i=5; i<=8; i++){
   nxshield.ledSetRGB(i,0,0);
   delay(GLOBAL_DELAY/2);
 } 
}
