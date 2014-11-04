/*
  Copyright (c) 2014 MediaTek Inc.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
   See the GNU Lesser General Public License for more details.
*/

#include <LTask.h>
#include <LWiFi.h>

void setup()
{
  LTask.begin();
  Serial.begin(115200);
}

// scan WiFi AP every 5 secs
void loop()
{
  // turn on wifi
  LWiFi.begin();
  Serial.println("scan start");
  int apNum = LWiFi.scanNetworks();
  Serial.println("scan end");
  Serial.println(apNum);
  for (int i = 0; i < apNum; ++i)
  {
    Serial.print("AP ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(LWiFi.SSID(i));
    Serial.print(" dB=");
    Serial.println(LWiFi.RSSI(i));
  }

  // turn off wifi
  LWiFi.end();
  Serial.println("Finish");
  delay(5000);
}


