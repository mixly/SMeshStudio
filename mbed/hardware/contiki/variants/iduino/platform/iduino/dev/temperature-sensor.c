

#include "contiki.h"
#include "dev/temperature-sensor.h"


/*---------------------------------------------------------------------------*/
static int
value(int type)
{
	return sensor_temp_get(type);
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
SENSORS_SENSOR(temperature_sensor, TEMPERATURE_SENSOR,
               value, configure, status);
