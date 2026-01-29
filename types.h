#ifndef TYPES_H
#define TYPES_H
#define MAX_TRACK 30 // TCAS II version 7.1 can track up to 30 transponder-equipped aircraft simultaneously.

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    THREAT_NONE = 0, 
    THREAT_TA   = 1, 
    THREAT_RA   = 2
} ThreatLevel;

typedef struct {
    double x, y, z;    // absolute position in meters
    double vx, vy, vz; // velocity components in meters/seconds, vz->vertical speed
} AircraftState;

typedef struct {    
    uint32_t mode_s_code;

    AircraftState state;
} OwnShip; 

typedef struct {
    bool isActive; 
    uint32_t mode_s_code;

    double relative_altitude; // meters
    double distance;          // meters
    double bearing;           // radians
    double closure_rate;      // meters/seconds

    AircraftState state;
    uint8_t threat_level; // holds ThreatLevel (size optimized with uint8_t)
} IntruderShip;

typedef struct {
    OwnShip host_aircraft;
    IntruderShip intruders[MAX_TRACK];
    pthread_mutex_t state_access_lock;
} SimulationState;

#endif