#include "lib/sensors.h"
#include "interrupt.h"
#include "dev/button-sensor.h"
const struct sensors_sensor button_sensor;
static struct timer debouncetimer;
struct sensors_sensor *sensors[] = {
		&button_sensor,
		0
};
unsigned char sensors_flags[1];
static int enabled = 0;
ISR(INT7_vect) {
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
			interrupt_init(INT7,0);
			interrupt_enable(INT7);
			enabled = 1;
			return 1;

		} else {
			enabled = 0;
			interrupt_disable(INT7);
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


