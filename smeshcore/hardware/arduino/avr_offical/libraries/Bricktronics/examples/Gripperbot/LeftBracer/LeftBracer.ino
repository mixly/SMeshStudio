#include <Wire.h>
#include <ArduinoNunchuk.h>

// Make: LEGO and Arduino Projects
// Chapter 7: GripperBot: Left Bracer
// Website: http://www.wayneandlayne.com/bricktronics/

// This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

// You need to install the ArduinoNunchuck library from https://github.com/GabrielBianconi/ArduinoNunchuk
ArduinoNunchuk nunchuk = ArduinoNunchuk();

#define NUM_OF_READINGS 10

char pitch_index = 0;
int pitch_readings[NUM_OF_READINGS];
int pitch_total = 0;

void setup()
{
    Serial.begin(9600);
    init_nunchuk_connections();
    delay(100);
    nunchuk.init();

    for (int i = 0; i < NUM_OF_READINGS; i++)
    {
        pitch_readings[i] = 0;
    }
}

void loop()
{
    nunchuk.update();
    handle_left_tank_tread(nunchuk.accelY);
    delay(50);
}

void init_nunchuk_connections()
{
    pinMode(A2, OUTPUT);
    digitalWrite(A2, LOW);
    pinMode(A3, OUTPUT);
    digitalWrite(A3, HIGH);
}

int average_accel_pitch(int raw_pitch)
{

    pitch_total -= pitch_readings[pitch_index];
    pitch_readings[pitch_index] = raw_pitch;
    pitch_total += pitch_readings[pitch_index];

    pitch_index += 1;
    if (pitch_index >= NUM_OF_READINGS)
    {
        pitch_index = 0;
    }

    return pitch_total / NUM_OF_READINGS;
}

int last_left_speed = 0;

void handle_left_tank_tread(int raw_accel_pitch)
{
    int avg_pitch = average_accel_pitch(raw_accel_pitch);

#define PITCH_CENTER 550
#define MAX_PITCH PITCH_CENTER+150
#define MIN_PITCH PITCH_CENTER-150

#define MAX_FORWARD 127
#define MAX_BACKWARD -128

    avg_pitch = constrain(avg_pitch, MIN_PITCH, MAX_PITCH);
    int left_speed = map(avg_pitch, MIN_PITCH, MAX_PITCH, MAX_BACKWARD, MAX_FORWARD);
    left_speed = constrain(left_speed, MAX_BACKWARD, MAX_FORWARD);

    if (left_speed != last_left_speed)
    {
        last_left_speed = left_speed;
        Serial.write('*');
        Serial.write(6);
        char out = left_speed;
        Serial.write(out);
    }
}
