#include <Wire.h>
#include <Bricktronics.h>

// Make: LEGO and Arduino Projects
// Chapter 5: Chocolate Milk Maker
// Website: http://www.wayneandlayne.com/bricktronics/

//This file was originally commented for print.  It will be recommented inline shortly, and the example will be updated in a new library update.

#define PUMP 13
#define STIR 11

#define SYRUP_WAIT 30000
#define PUMP_TIME 22000
#define STIR_TIME 20000

Bricktronics brick = Bricktronics();
PIDMotor syrup_arm = PIDMotor(&brick, 1);
PIDMotor stir_arm = PIDMotor(&brick, 2);
Button startstop = Button(&brick, 1);
Button endstop = Button(&brick, 2);

void setup()
{
    Serial.begin(115200);
    Serial.println("starting!");
    brick.begin();
    syrup_arm.begin();
    stir_arm.begin();
    startstop.begin();
    endstop.begin();
    setup_loads();
}

void setup_loads()
{
    pinMode(PUMP, OUTPUT);
    digitalWrite(PUMP, LOW);

    pinMode(STIR, OUTPUT);
    digitalWrite(STIR, LOW);
}

void start_pump()
{
    digitalWrite(PUMP, HIGH);
}

void stop_pump()
{
    digitalWrite(PUMP, LOW);
}

void start_stir()
{
    digitalWrite(STIR, HIGH);
}

void start_stir(int speed)
{
    analogWrite(STIR, speed);
}

void stop_stir()
{
    digitalWrite(STIR, LOW);
}

void loop()
{
    Serial.println("Starting loop!");

    wait_for_start_press_and_release();

    if (endstop.is_pressed())
    {
        Serial.println("Error. Endstop is already pressed at start of run.");
        return;
    }

    pump_milk();
    dispense_syrup();
    drop_stir_arm();
    stir_chocolate_milk();
    raise_stir_arm();
}

void wait_for_start_press_and_release()
{
    Serial.println("Waiting for start press.");
    while (!startstop.is_pressed()) {
        //wait for start to be pressed
    };
    delay(50); //debounce
    while (startstop.is_pressed()) {
        //wait for start to be released
    };
    delay(50); //debounce
    Serial.println("Start button released!");
}

void pump_milk()
{
    Serial.println("Starting pump.");
    start_pump();

    unsigned long end_time = millis() + PUMP_TIME;
    while (millis() < end_time) {
        if (startstop.is_pressed())
        {
            Serial.println("Pump stopped due to button press.");
            break;
        }
        delay(50);
    }
    stop_pump();
}

void dispense_syrup()
{
    Serial.println("Advancing syrup arm until endstop.");
    syrup_arm.set_speed(255);
    while (!endstop.is_pressed()) {
    };
    syrup_arm.encoder->write(0);
    syrup_arm.stop();
    Serial.println("Endstop pressed!");
    Serial.println("Waiting and dispensing syrup.");

    for (int i = 0; i < 40; i++)
    {
        syrup_arm.go_to_pos(-100);
        Bricktronics::delay_update(SYRUP_WAIT/80, &syrup_arm);
        syrup_arm.go_to_pos(0);
        Bricktronics::delay_update(SYRUP_WAIT/80, &syrup_arm);
    }

    Serial.println("Retreating syrup arm!");
    syrup_arm.go_to_pos(20);
    Bricktronics::delay_update(100, &syrup_arm);
    syrup_arm.go_to_pos(50);
    Bricktronics::delay_update(100, &syrup_arm);
    syrup_arm.go_to_pos(225);
    Bricktronics::delay_update(1000, &syrup_arm);
    syrup_arm.go_to_pos(350);
    Bricktronics::delay_update(1000, &syrup_arm);
    syrup_arm.stop();
}

void drop_stir_arm()
{
    Serial.println("Advancing stir arm until endstop.");
    stir_arm.set_speed(-100);

    while (!endstop.is_pressed()) {
        //do nothing
    };

    stir_arm.encoder->write(0);
    stir_arm.stop();
    Serial.println("Endstop pressed!");
}

void stir_chocolate_milk()
{
    Serial.println("Starting to stir");
    start_stir(255);
    unsigned long end_time = millis() + STIR_TIME;
    while (millis() < end_time) {
        if (startstop.is_pressed())
        {
            Serial.println("Stir stopped due to button press.");
            break;
        }
        delay(50);
    }
    stop_stir();
}

void raise_stir_arm()
{
    Serial.println("Retreating stir arm!");

    stir_arm.go_to_pos(-35);
    Bricktronics::delay_update(1000, &stir_arm);
    stir_arm.go_to_pos(-60);
    Bricktronics::delay_update(2000, &stir_arm);
    stir_arm.go_to_pos(-100);
    start_stir(85);
    Bricktronics::delay_update(2000, &stir_arm);
    stop_stir();

    stir_arm.go_to_pos(-110);
    Bricktronics::delay_update(2000, &stir_arm);
    stir_arm.go_to_pos(-250);
    Bricktronics::delay_update(1000, &stir_arm);
    stir_arm.stop();
}
