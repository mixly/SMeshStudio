#include <Wire.h>
#include <BricktronicsMegashield.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>

// Make: LEGO and Arduino Projects
// Chapter 9: Lamp
// Website: http://www.wayneandlayne.com/bricktronics/

// Based on BluetoothShield Demo Code PDE by Steve Chang at Seeed Technology, Inc.
// and
// AC Light Control by Ryan McLaughlin.

// This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

BricktronicsMegashield brick = BricktronicsMegashield();
PIDMotor m = PIDMotor(&brick, 1);
Button lamp_switch = Button(&brick, 1);

#define RxD 6
#define TxD 7
#define AC_pin 9

volatile int i = 0;
volatile boolean zero_cross = false;

int dim = 0;
int freqStep = 65;

long last_encoder_reading = 0;
boolean lamp_lit = false;
long last_released = 0;
int destination_updates;

SoftwareSerial bluetoothSerial(RxD,TxD);

void setup()
{
    Serial.begin(9600);
    setupBluetoothConnection();
    brick.begin();
    m.begin();
    lamp_switch.begin();

    m.mKP = 1;
    m.mKD = 1;
    m.mKI = 0.003;

    pinMode(AC_pin, OUTPUT);
    attachInterrupt(1, zero_cross_detect, FALLING);
    Timer1.initialize(freqStep);
    Timer1.attachInterrupt(dim_check, freqStep);
}


void loop()
{
    char recvChar;
    if (bluetoothSerial.available())
    {
        recvChar = bluetoothSerial.read();
        switch (recvChar)
        {
        case 'q':
            if (lamp_lit)
            {
                bluetoothSerial.print('l');
            } else
            {
                bluetoothSerial.print('m');
            }
            bluetoothSerial.print((char) dim);
            break;
        case 'm':
            lamp_lit = false;
            dim = 128;
            break;
        case 'l':
            lamp_lit = true;
            dim = 0;
            break;
        default:
            if (recvChar >= 0 && recvChar <=99)
            {
                Serial.println("Setting dimness:");
                dim = recvChar;
                destination_updates = 20;
                m.go_to_pos(dim*4);
            }
        }
    }

    if (destination_updates)
    {
        m.update();
        delay(25);
        destination_updates -= 1;
    } else
    {
        m.stop();
    }

    long current_encoder_reading = m.encoder->read();

    if (current_encoder_reading != last_encoder_reading)
    {
        dim = current_encoder_reading / 4;
        Serial.print("dim: ");
        Serial.println(dim);
        dim = constrain(dim, 0, 100);
        bluetoothSerial.write((byte)dim);
        last_encoder_reading = current_encoder_reading;
    }

    if (! lamp_switch.is_pressed())
    {
        last_released = millis();
    }
    else if (lamp_switch.is_pressed() && last_released > 0
             && last_released - millis() > 50)
    {
        Serial.println("pressed");
        last_released = 0;
        if (lamp_lit)
        {
            lamp_lit = false;
            dim = 128;
            bluetoothSerial.print("m");
        }
        else {
            lamp_lit = true;
            dim = 0;
            bluetoothSerial.print("l");
        }
    }

    if (Serial.available())
    {
        recvChar  = Serial.read();
        bluetoothSerial.print(recvChar);
    }
}

void setupBluetoothConnection()
{
    bluetoothSerial.begin(9600);
    bluetoothSerial.print("\r\n+STWMOD=0\r\n");
    bluetoothSerial.print("\r\n+STNA=BricktronicsMegashield\r\n");
    bluetoothSerial.print("\r\n+STOAUT=1\r\n");
    bluetoothSerial.print("\r\n+STAUTO=0\r\n");
    delay(2000);
    bluetoothSerial.print("\r\n+INQ=1\r\n");
    Serial.println("The Bluetooth module should be connectable!");
    delay(2000);
    bluetoothSerial.flush();
}

void zero_cross_detect()
{
    zero_cross = true;
}

void dim_check()
{
    if (zero_cross && lamp_lit)
    {
        if (i >= dim)
        {
            digitalWrite(AC_pin, HIGH);
            delayMicroseconds(5);
            digitalWrite(AC_pin, LOW);
            i = 0;
            zero_cross = false;
        }
        else
        {
            i++;
        }
    }
}

