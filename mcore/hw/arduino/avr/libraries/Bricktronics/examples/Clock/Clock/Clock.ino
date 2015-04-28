#include <Wire.h>
#include <Bricktronics.h>
#include <Time.h>

// Make: LEGO and Arduino Projects
// Chapter 4: Clock
// Website: http://www.wayneandlayne.com/bricktronics/

//This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

// The minimum number of milliseconds between calls to loop()
#define TIME_STEP 100

time_t t;

Bricktronics brick = Bricktronics();
PIDMotor h = PIDMotor(&brick, 1);
PIDMotor m = PIDMotor(&brick, 2);
Button hour_button = Button(&brick, 1);
Button minute_button = Button(&brick, 2);

void setup()
{
    Serial.begin(115200);
    Serial.println("starting!");

    brick.begin();
    m.begin();
    h.begin();
    hour_button.begin();
    minute_button.begin();
}

void digitalClockDisplay()
{
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
}

void printDigits(int digits)
{
    Serial.print(":");
    if (digits < 10)
    {
        Serial.print('0');
    }
    Serial.print(digits);
}

void increment_minute()
{
    adjustTime(60);
    Serial.println("Advance minute pressed");
}

void increment_hour()
{
    adjustTime(3600);
    Serial.println("Advance hour pressed");
}

void check_buttons()
{
    static char last_minute_status = HIGH;
    static char last_hour_status = HIGH;

    char minute_status = minute_button.is_pressed();
    char hour_status = hour_button.is_pressed();

    if (minute_status == HIGH && last_minute_status == LOW)
    {
        increment_minute();
    }

    if (hour_status == HIGH && last_hour_status == LOW)
    {
        increment_hour();
    }

    last_minute_status = minute_status;
    last_hour_status = hour_status;
}


void loop()
{
    long next_loop = millis() + TIME_STEP;
    check_buttons();
    t = now();

    int full_hours = hour(t) % 12;
    int minute_position;
    minute_position = minute(t)*6 + 360*full_hours;

    int m_pos;
    m_pos = -3 * 2 * minute_position;

    int hour_position;
    hour_position = (hour(t) % 12) * 30 + minute(t)/2;
    int h_pos = 2 * 5 * hour_position;
    digitalClockDisplay();
    Serial.println();

    m.go_to_pos(m_pos);
    h.go_to_pos(h_pos);

    while (millis() < next_loop)
    {
        h.update();
        m.update();
        delay(50);
    }
}
