#include "MxRadio.h"
const uint16_t  localaddress=1;
const uint16_t  destaddress=2;
const channel_t curchannel=11;
uint8_t lastSendData[110];
uint8_t lastSendDataLen;
uint8_t bufIndex=0;
void onXmitDone(radio_tx_done_t x)
{
	if (x!=TX_OK)
	{
		delay(50);
		MxRadio.beginTransmission(destaddress);
		MxRadio.write(lastSendData,bufIndex);
		MxRadio.endTransmission();
	}
}
void setup()
{
	MxRadio.begin(curchannel,0xffff,localaddress,true,true,true);
	Serial.begin(38400);
	MxRadio.attachTxDone(onXmitDone);
}

void loop()
{

	if (Serial.available())
	{
		bufIndex=0;
		while (Serial.available())
		{
			lastSendData[bufIndex]=(uint8_t)Serial.read();
			bufIndex++;
			if (bufIndex>110)
				break;
		}
		MxRadio.beginTransmission(destaddress);
		MxRadio.write(lastSendData,bufIndex);
		MxRadio.endTransmission();
	}

	if (MxRadio.available())
	{
		while(MxRadio.available())
			Serial.write(MxRadio.read());
	}
}
