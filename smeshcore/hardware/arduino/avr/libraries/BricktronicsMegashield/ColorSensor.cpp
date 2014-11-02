#include "ColorSensor.h"

ColorSensor::ColorSensor(BricktronicsMegashield* b, uint8_t port)
{
    bms = b;

    //TODO: do this a better way
    switch (port)
    {
        case 1:
            _clock = S1_DA;
            _data = S1_ANA;
            break;
        case 2:
            _clock = S2_DA;
            _data = S2_ANA;
            break;
        case 3:
            _clock = S3_DA;
            _data = S3_ANA;
            break;
        case 4:
            _clock = S4_DA;
            _data = S4_ANA;
            break;
    }
}

void ColorSensor::begin(void)
{
    begin(TYPE_COLORFULL);
}

void ColorSensor::begin(uint8_t modetype)
{
    mode = modetype;
    type = modetype;

    reset_sensor();
    send_mode(mode);
    delayMicroseconds(3200);
    read_calibration();
    delay(120); //This can be removed if you have other setup code that doesn't use the color sensor that takes a while.
}


void ColorSensor::set_clock(int val)
{
    pinMode(_clock, OUTPUT);
    digitalWriteFast2(_clock, val);
}

void ColorSensor::set_data(int val)
{
    pinMode(_data, OUTPUT);
    digitalWriteFast2(_data, val);
}

bool ColorSensor::get_data()
{
    pinMode(_data, INPUT);
    bool data = digitalReadFast2(_data) != 0;
    return data;
}

int ColorSensor::read_data()
{
    pinMode(_data, INPUT);
    digitalWriteFast2(_data, LOW);
    int data = analogRead(_data);
    return data*50/33;
}

void ColorSensor::reset_sensor()
{
    set_clock(1);
    set_data(1);
    delay(1);
    set_clock(0);
    delay(1);
    set_clock(1);
    delay(1);
    set_clock(0);
    delay(100);
}

void ColorSensor::send_mode(unsigned int mode)
{
    for (int i = 0; i < 8; i++)
    {
        set_clock(1);
        set_data(mode & 1);
        delayMicroseconds(30);
        set_clock(0);
        mode >>= 1;
        delayMicroseconds(30);
    }
}

char ColorSensor::read_byte()
{
    //This is way slower than how LEGO does it... tens of microseconds slower!  When I tested this matching the speed the LEGO does it, I had some issues.  Look into this. TODO
    unsigned char val = 0;
    for (int i = 0; i< 8; i++)
    {
        set_clock(1);
        val >>= 1;
        if (get_data())
        {
            val |= 0x80;
        }
        set_clock(0);
    }
    return val;
}

unsigned int ColorSensor::calc_CRC(unsigned int crc, unsigned int val)
{
    for (int i = 0; i < 8; i++)
    {
        if (((val ^ crc) & 1) != 0)
        {
            crc = ((crc >> 1) ^ 0xa001); // the >> should shift a zero in
        }
        else
        {
            crc >>= 1; //the >> should shift a zero in
        }
        val >>= 1; //the >> should shift a zero in
    }
    return crc & 0xffff;
}

bool ColorSensor::read_calibration()
{
    uint16_t crcVal = 0x5aa5;
    uint8_t input;
    pinMode(_data, INPUT);
    for (int i = 0; i < CAL_ROWS; i++)
    {
        for (int col = 0; col < CAL_COLUMNS; col++)
        {
            uint32_t val = 0;
            uint8_t shift_val = 0;
            for (int k = 0; k < 4; k++)
            {
                input = read_byte();
                crcVal = calc_CRC(crcVal, input);
                val |= ((uint32_t) input << shift_val);
                shift_val += 8;
            }
            calData[i][col] = val;
        }
    }
    for (int i = 0; i < CAL_LIMITS; i++)
    {
        unsigned long val = 0;
        int shift = 0;
        for (int k = 0; k < 2; k++)
        {
            input = read_byte();
            crcVal = calc_CRC(crcVal, input);
            val |= input << shift;
            shift += 8;
        }

        calLimits[i] = val;
    }
    unsigned int crc = (read_byte() << 8);
    crc += read_byte();
    crc &= 0xffff;
    delay(2);
    return true;
}

bool ColorSensor::check_sensor()
{
    pinMode(_clock, INPUT);
    delay(2);
    if (digitalRead(_clock))
    {
        return false; // error
    }
    else
    {
        return true; // not error
    }
}

int ColorSensor::read_full_color_value(int newClock)
{
    delayMicroseconds(10);
    int val = read_data();
    int val2 = read_data();
    set_clock(newClock);
    return (val+val2)/2;
}


void ColorSensor::read_sensor()
{
    if (type == TYPE_COLORFULL)
    {
        raw_values[INDEX_BLANK] = read_full_color_value(1);
        raw_values[INDEX_RED] = read_full_color_value(0);
        raw_values[INDEX_GREEN] = read_full_color_value(1);
        raw_values[INDEX_BLUE] = read_full_color_value(0);
    }
    else
    {
        raw_values[type - TYPE_COLORRED] = read_data();
    }
}

int ColorSensor::read_raw_value()
{
    if (type < TYPE_COLORRED)
    {
        return -1;
    }
    read_sensor();
    return raw_values[type - TYPE_COLORRED];
}

int ColorSensor::calibrate()
{
    // calibrate raw_values to cal_values
    int cal_tab;
    int blank_val = raw_values[INDEX_BLANK];
    if (blank_val < calLimits[1])
    {
        cal_tab = 2;
    }
    else if (blank_val < calLimits[0])
    {
        cal_tab = 1;
    }
    else
    {
        cal_tab = 0;
    }

    for (int col = INDEX_RED; col <= INDEX_BLUE; col++)
    {
        if (raw_values[col] > blank_val)
        {
            cal_values[col] = ((raw_values[col] - blank_val) * calData[cal_tab][col]) >> 16; // TODO check shift!
        }
        else
        {
            cal_values[col] = 0;
        }
    }
        
    if (blank_val > MINBLANKVAL)
    {
        blank_val -= MINBLANKVAL;
    }
    else
    {
        blank_val = 0;
    }

    blank_val = (blank_val * 100) / (((SENSORMAX - MINBLANKVAL ) * 100) / ADMAX);

    cal_values[INDEX_BLANK] = (blank_val * calData[cal_tab][INDEX_BLANK]) >> 16 ; // TODO CHECK SHIFT
}

void ColorSensor::print_color(uint8_t color)
{
    //TODO: use PROGMEM for these strings
    switch (color)
    {
        case COLOR_BLACK:
            Serial.print("black");
            break;
        case COLOR_BLUE:
            Serial.print("blue");
            break;
        case COLOR_GREEN:
            Serial.print("green");
            break;
        case COLOR_YELLOW:
            Serial.print("yellow");
            break;
        case COLOR_RED:
            Serial.print("red");
            break;
        case COLOR_WHITE:
            Serial.print("white");
            break;
        default:
            Serial.println("error");
    }
}


uint8_t ColorSensor::cal_to_color()
{
    int red = cal_values[INDEX_RED];
    int blue = cal_values[INDEX_BLUE];
    int green = cal_values[INDEX_GREEN];
    int blank = cal_values[INDEX_BLANK];

    // The following algorithm comes from the 1.29 Lego firmware.
    if (red > blue && red > green)
    {
        // red dominant color
        if (red < 65 || (blank < 40 && red < 110))
            return COLOR_BLACK;
        if ( ((blue >> 2) + (blue >> 3) + blue < green) && ((green << 1) > red))
            return COLOR_YELLOW;
        if ((green << 1) - (green >> 2) < red)
            return COLOR_RED;
        if (blue < 70 || green < 70 || (blank < 140 && red < 140))
            return COLOR_BLACK;
        return COLOR_WHITE;
    }
    else if (green > blue)
    {
        // green dominant color
        if (green < 40 || (blank < 30 && green < 70))
            return COLOR_BLACK;
        if ((blue << 1) < red)
            return COLOR_YELLOW;
        if ((red + (red >> 2)) < green || (blue + (blue>>2)) < green )
            return COLOR_GREEN;
        if (red < 70 || blue < 70 || (blank < 140 && green < 140))
            return COLOR_BLACK;
        return COLOR_WHITE;
    }
    else
    {
        // blue dominant color
        if (blue < 48 || (blank < 25 && blue < 85))
            return COLOR_BLACK;
        if ((((red*48) >> 5) < blue && ((green*48) >> 5) < blue) || ((red*58) >> 5) < blue || ((green*58) >> 5) < blue)
            return COLOR_BLUE;
        if (red < 60 || green < 60 || (blank < 110 && blue < 120))
            return COLOR_BLACK;
        if ((red + (red >> 3)) < blue || (green + (green >> 3)) < blue)
            return COLOR_BLUE;
        return COLOR_WHITE;
    }
}

uint8_t ColorSensor::get_color()
{
    read_sensor();
    calibrate();
    return cal_to_color();
}

