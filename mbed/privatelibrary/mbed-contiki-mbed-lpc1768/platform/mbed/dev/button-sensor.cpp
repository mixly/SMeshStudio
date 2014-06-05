
#ifdef __cplusplus
extern "C" {
#endif
#include "lib/sensors.h"
#include "dev/button-sensor.h"
#ifdef __cplusplus
}
#endif
#include "mbed.h"

//const struct sensors_sensor button_sensor;
static struct timer debouncetimer;
struct sensors_sensor *sensors[] = {
		(sensors_sensor*)&button_sensor,
		0
};
unsigned char sensors_flags[1];
static int enabled = 0;
InterruptIn  userbutton(P2_8);

void mybtnhandler() {
	if(timer_expired(&debouncetimer)) {
		timer_set(&debouncetimer, CLOCK_SECOND / 16);
		sensors_changed(&button_sensor);
	}

}


static int
value(int type)
{
	return 0;
}

static int
configure(int type, int c)
{
	switch (type) {
	case SENSORS_ACTIVE:
		if (c) {
			timer_set(&debouncetimer, 0);
			userbutton.fall(&mybtnhandler);
			enabled = 1;
			return 1;

		} else {
			enabled = 0;

		}
		return 1;
	}
	return 0;
}

static int
status(int type)
{
	switch (type) {
	case SENSORS_ACTIVE:

	case SENSORS_READY:
		return 1;
	}
	return 0;
}

SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
		value, configure, status);


