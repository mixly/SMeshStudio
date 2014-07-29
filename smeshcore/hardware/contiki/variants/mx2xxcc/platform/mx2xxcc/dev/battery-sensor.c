
#include "contiki.h"
#include "dev/battery-sensor.h"
#include "adc-sensors.h"
const struct sensors_sensor battery_sensor;
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
	return voltage_get();
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
	return 1;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
	return 1;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR, value, configure, status);
