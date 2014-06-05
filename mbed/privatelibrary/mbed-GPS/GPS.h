/******************************************************
* A basic mbed library for the Adafruit ultimate      *
* GPS module NOTE: the LOCUS functions are yet to be  *
* implemented                                         *
*                                                     *
*                              Written by: Sam Clarke *
******************************************************/

#include "mbed.h"
#include <string>

#ifndef GPS_H
#define GPS_H

// EXAMPLE OUTPUTS
//
// $GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C
// $GPRMC, time, status, latitude, N/S, longitude, E/W, speed(knots), heading, date, N/A, N/A, MODE*CHECKSUM
//
// $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*65
// $GPGGA, time, latitude, N/S, longitude, E/W, fix, satellites, hdop, altitude, M, geoidal sep , M,,*CHECKSUM
// $GPGGA, %f, %*f, %*c, %*f, %*c, %d, %d, %*f, %*f, %*c, %*f , %*c,,%*c%*c%*c0

class GPS
{
public:

    GPS(PinName tx, PinName rx);
    void Init();
    int parseData();
    float time;         // UTC time
    int hours;
    int minutes;
    float seconds;
    char validity,ns,ew;// RMC data status A = Data Valid; V = Data Not valid;
    float latitude;     //
    float longitude;    //
    float speed;        // speed in knots
    float heading;      // heading in degrees derived from previous & current location
    int date;           //
    int fixtype;        // 0 = no fix;  1 = fix;  2=differential fix
    int satellites;     // number of satellites used
    float altitude;     //
    string fix; 
    float kph;

private:

    float trunc ( float v);
    void getData();
    Serial _UltimateGps;
    char NEMA[256];
};
#endif

/*
#define 1HZ_STREAM  "$PMTK220,1000*1F\r\n"  // 1.0 second interval
#define 5HZ_STREAM  "$PMTK220,200*2C\r\n"   // 0.2 second interval
#define 10HZ_STREAM "$PMTK220,100*2F\r\n"   // 0.1 second interval

#define OUTPUT_RMC    "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
#define OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define OUTPUT_OFF    "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
*/