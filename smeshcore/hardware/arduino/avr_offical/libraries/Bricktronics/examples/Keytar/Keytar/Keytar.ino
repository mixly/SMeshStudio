#include <Wire.h>
#include <Bricktronics.h>

// Make: LEGO and Arduino Projects
// Chapter 8: Keytar
// Website: http://www.wayneandlayne.com/bricktronics/

//This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

Bricktronics brick = Bricktronics();
Motor tone_knob = Motor(&brick, 1);
Motor duration_knob = Motor(&brick, 2);
Button mute_button = Button(&brick, 1);

#define SPEAKER 11
#define TONE_SLOT_0 6
#define TONE_SLOT_1 7
#define TONE_SLOT_2 A1
#define TONE_SLOT_3 A2
#define TONE_SLOT_4 A3

#define NUM_OF_SLOTS 5

char slot_button[] = {
    TONE_SLOT_0,
    TONE_SLOT_1,
    TONE_SLOT_2,
    TONE_SLOT_3,
    TONE_SLOT_4
};

int slot_frequency[] = {300, 400, 500, 600,700};
int note_duration = 500;
int rest_duration = 100;
int current_slot = 0;


#define MIN_FREQUENCY 25
#define MIN_NOTE_DURATION 25

void setup()
{
    Serial.begin(115200);
    Serial.println("starting!");
    brick.begin();
    tone_knob.begin();
    duration_knob.begin();
    mute_button.begin();

    for (char i = 0; i < NUM_OF_SLOTS; i++)
    {
        pinMode (slot_button[i], INPUT_PULLUP);
    }

    while (!mute_button.is_pressed()) {
        //do nothing
    }
}


void loop()
{
    Serial.println(current_slot);
    int current_frequency = slot_frequency[current_slot];

    if (!mute_button.is_pressed())
    {
        tone(SPEAKER, current_frequency);
    }

    watch_for_input(note_duration, current_slot, false, true);
    noTone(SPEAKER);
    watch_for_input(rest_duration, current_slot, false, false);

    current_slot += 1;

    if (current_slot == NUM_OF_SLOTS)
    {
        current_slot = 0;
    }
}


void watch_for_input(long ms, char slot, boolean in_slot, boolean is_playing)
{
    long start_time = millis();
    long end_time = start_time + ms;
    tone_knob.encoder->write(0);
    duration_knob.encoder->write(0);
    Serial.print("In slot: ");
    Serial.println(in_slot, DEC);
    while (millis() < end_time)
    {
        if (!in_slot)
        {
            for (char i = 0; i < NUM_OF_SLOTS; i++)
            {
                // TODO should this be brick.digitalRead ?
                if (digitalRead(slot_button[i]) == LOW)
                {
                    Serial.print("Button pressed: ");
                    Serial.println(i, DEC);
                    handle_button_press(i);
                }
            }
        }

        long duration_change = duration_knob.encoder->read();

        if (duration_change != 0)
        {
            Serial.print("duration knob: ");
            Serial.println(duration_change, DEC);
            if (in_slot)
            {
                note_duration += duration_change;
                note_duration = max(note_duration, MIN_NOTE_DURATION);
            }
            else
            {
                rest_duration += duration_change;
                rest_duration = max(rest_duration, 0);
            }
            end_time = millis();
            duration_knob.encoder->write(0);
        }

        if (tone_knob.encoder->read() != 0)
        {
            Serial.print("Tone knob: ");
            Serial.println(tone_knob.encoder->read(), DEC);

            slot_frequency[slot] += (tone_knob.encoder->read());
            slot_frequency[slot] = max(MIN_FREQUENCY, slot_frequency[slot]);
            if (is_playing && !mute_button.is_pressed())
            {
                tone(SPEAKER, slot_frequency[slot]);
            }
            tone_knob.encoder->write(0);
        }
    }
}

void handle_button_press(char slot)
{
    Serial.print("Handling button press for slot: ");
    Serial.println(slot);
    noTone(SPEAKER);

    // TODO should this be brick.digitalRead ?
    while (digitalRead(slot_button[slot]) == LOW)
    {
        tone(SPEAKER, slot_frequency[slot]);
        watch_for_input(note_duration, slot, true, true);
        noTone(SPEAKER);
        watch_for_input(rest_duration, slot, true, false);
    }
}
