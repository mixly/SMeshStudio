#include "MxRadio.h"
uint16_t  localaddress=1;
uint16_t  destaddress=2;
channel_t curchannel=11;
void errHandle(radio_error_t err)
{
	Serial.println();
	Serial.print("Error: ");
	Serial.print((uint8_t)err, 10);
	Serial.println();
}

void onXmitDone(radio_tx_done_t x)
{
	Serial.println();
	Serial.print("TxDone: ");
	Serial.print((uint8_t)x, 10);
	Serial.println();
}
void setup()
{
	MxRadio.begin(curchannel,0xffff,localaddress,true,true,true);
	Serial.begin(38400);
	MxRadio.attachError(errHandle);
	MxRadio.attachTxDone(onXmitDone);
}
void loop()
{
	MxRadio.beginTransmission(destaddress);
	MxRadio.write("hello world from ");
	MxRadio.write(localaddress+'0');
	MxRadio.write("!");
	MxRadio.endTransmission();
	delay(2000);
	if (MxRadio.available())
	{
		Serial.println();
		Serial.print("Rx: ");
		while(MxRadio.available())
			Serial.write(MxRadio.read());

		Serial.println();
		Serial.print("LQI: ");
		Serial.print(MxRadio.getLqi(), 10);
		Serial.print(", RSSI: ");
		Serial.print(MxRadio.getLastRssi(), 10);
		Serial.print(" dBm, ED: ");
		Serial.print(MxRadio.getLastEd(), 10);
		Serial.println("dBm");
	}
	delay(100);
}
