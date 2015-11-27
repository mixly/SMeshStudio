
#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <stdint.h>

typedef struct MotionSensorDataCounts
{
    int16_t x, y, z;
} MotionSensorDataCounts;

typedef struct MotionSensorDataUnits
{
    float x, y, z;
} MotionSensorDataUnits;
 
class MotionSensor
{
public:
    //virtual MotionSensor();
    virtual void enable(void) = 0;
    virtual void disable(void) = 0;
    virtual uint32_t sampleRate(uint32_t frequency) = 0;
    virtual uint32_t whoAmI(void) = 0;
    virtual uint32_t dataReady(void) = 0;
    virtual void getX(int16_t * x) = 0;
    virtual void getY(int16_t * y) = 0;
    virtual void getZ(int16_t * z) = 0;
    virtual void getX(float * x) = 0;
    virtual void getY(float * y) = 0;
    virtual void getZ(float * z) = 0;
    virtual void getAxis(MotionSensorDataCounts &data) = 0;
    virtual void getAxis(MotionSensorDataUnits &data) = 0;
};

#endif    
