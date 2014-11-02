#include <Wire.h>
#include <ArduinoNunchuk.h>

// Make: LEGO and Arduino Projects
// Chapter 7: GripperBot: Right Bracer
// Website: http://www.wayneandlayne.com/bricktronics/

// This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

// You need to install the ArduinoNunchuck library from https://github.com/GabrielBianconi/ArduinoNunchuk
ArduinoNunchuk nunchuk = ArduinoNunchuk();

#define NUM_OF_READINGS 10

char roll_index = 0;
int roll_readings[NUM_OF_READINGS];
int roll_total = 0;
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
        roll_readings[i] = 0;
        pitch_readings[i] = 0;
    }
}


void loop()
{
    nunchuk.update();
    handle_right_tank_tread(nunchuk.accelY);
    handle_wrist(nunchuk.accelX);
    handle_spin(nunchuk.analogX);
    handle_elbow(nunchuk.analogY);
    handle_grip(nunchuk.zButton, nunchuk.cButton);

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

int average_accel_roll(int raw_roll)
{
    roll_total -= roll_readings[roll_index];
    roll_readings[roll_index] = raw_roll;
    roll_total += roll_readings[roll_index];

    roll_index += 1;
    if (roll_index >= NUM_OF_READINGS)
    {
        roll_index = 0;
    }

    return roll_total / NUM_OF_READINGS;
}

int last_right_speed = 0;

void handle_right_tank_tread(int raw_accel_pitch)
{

    int avg_pitch = average_accel_pitch(raw_accel_pitch);

#define PITCH_CENTER 550

#define MAX_PITCH PITCH_CENTER+150
#define MIN_PITCH PITCH_CENTER-150

#define MAX_FORWARD 127
#define MAX_BACKWARD -128

    avg_pitch = constrain(avg_pitch, MIN_PITCH, MAX_PITCH);
    int right_speed = map(avg_pitch, MIN_PITCH, MAX_PITCH, MAX_BACKWARD, MAX_FORWARD);
    right_speed = constrain(right_speed, MAX_BACKWARD, MAX_FORWARD);


    if (right_speed != last_right_speed)
    {
        last_right_speed = right_speed;
        Serial.write('*');
        Serial.write(7);
        char out = right_speed;
        Serial.write(out);
    }
}

int last_spin_speed = 0;

void handle_spin(int joy_x)
{

#define JOY_X_CENTER 129

#define MAX_JOY_X JOY_X_CENTER+101
#define MIN_JOY_X JOY_X_CENTER-97

#define MAX_RIGHT_SPIN 127
#define MAX_LEFT_SPIN -128

    joy_x = constrain(joy_x, MIN_JOY_X, MAX_JOY_X);
    int spin_speed = map(joy_x, MIN_JOY_X, MAX_JOY_X, MAX_LEFT_SPIN, MAX_RIGHT_SPIN);
    spin_speed = constrain(spin_speed, MAX_LEFT_SPIN, MAX_RIGHT_SPIN);

    if (spin_speed != last_spin_speed)
    {
        last_spin_speed = spin_speed;
        Serial.write('*');
        Serial.write(1);
        char out = spin_speed;
        Serial.write(out);
    }
}

int last_elbow_speed = 0;

void handle_elbow(int joy_y)
{

#define JOY_Y_CENTER 126

#define MAX_JOY_Y JOY_Y_CENTER+86
#define MIN_JOY_Y JOY_Y_CENTER-98

#define MAX_UP_ELBOW 127
#define MAX_DOWN_ELBOW -128

    joy_y = constrain(joy_y, MIN_JOY_Y, MAX_JOY_Y);
    int elbow_speed = map(joy_y, MIN_JOY_Y, MAX_JOY_Y, MAX_UP_ELBOW, MAX_DOWN_ELBOW);
    elbow_speed = constrain(elbow_speed, MAX_DOWN_ELBOW, MAX_UP_ELBOW);

    if (elbow_speed != last_elbow_speed)
    {
        last_elbow_speed = elbow_speed;
        Serial.write('*');
        Serial.write(2);
        char out = elbow_speed;
        Serial.write(out);
    }
}

int last_wrist_speed = 0;

void handle_wrist(int raw_accel_roll)
{
    int avg_roll = average_accel_roll(raw_accel_roll);

#define ROLL_CENTER 560

#define MAX_ROLL ROLL_CENTER+220
#define MIN_ROLL ROLL_CENTER-220

#define MAX_RIGHT_WRIST 127
#define MAX_LEFT_WRIST -128

    avg_roll = constrain(avg_roll, MIN_ROLL, MAX_ROLL);
    int wrist_speed = map(avg_roll, MIN_ROLL, MAX_ROLL, MAX_LEFT_WRIST, MAX_RIGHT_WRIST);
    wrist_speed = constrain(wrist_speed, MAX_LEFT_WRIST, MAX_RIGHT_WRIST);

    if (wrist_speed != last_wrist_speed)
    {
        last_wrist_speed = wrist_speed;
        Serial.write('*');
        Serial.write(3);
        char out = wrist_speed;
        Serial.write(out);
    }
}

char last_cmd = 0;

void handle_grip(boolean z, boolean c)
{
#define GRIP_OUT_SPEED 100
#define GRIP_IN_SPEED -100

    if (z == c)
    {
        if (last_cmd != 0)
        {
            last_cmd = 0;
            Serial.write('*');
            Serial.write(4);
            Serial.write((unsigned byte) 0);
        }
    } else if (z) //out
    {
        if (last_cmd != 'z')
        {
            last_cmd = 'z';
            Serial.write('*');
            Serial.write(4);
            Serial.write(GRIP_OUT_SPEED);
        }
    } else if (c) //in
    {
        if (last_cmd != 'c')
        {
            last_cmd = 'c';
            Serial.write('*');
            Serial.write(4);
            Serial.write(GRIP_IN_SPEED);
        }
    }
}

