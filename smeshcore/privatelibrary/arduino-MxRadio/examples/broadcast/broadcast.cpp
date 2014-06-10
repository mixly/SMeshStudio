#include "MxRadio.h"
uint8_t i;
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
	MxRadio.begin(11);
	Serial.begin(9600);
	MxRadio.attachError(errHandle);
	MxRadio.attachTxDone(onXmitDone);
}
void loop()
{
	MxRadio.beginTransmission();
	MxRadio.write("Hello World!");
	MxRadio.write(i);
	i++;
	MxRadio.endTransmission();
	delay(1000);
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
}
