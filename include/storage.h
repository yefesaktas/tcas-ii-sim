#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdatomic.h>

#include "types.h"

// flag variable that will be used for graceful shutdown
extern volatile atomic_bool SIGINT_signaled;

// function prototypes
void init_buffer(void);
void finalize_buffer(void);
void set_OwnShip_state(const AircraftState* state, uint32_t mode_s_code);
void set_IntruderShip_state(int index, const AircraftState* state, uint32_t mode_s_code);
void update_intruder_data(int index, double relative_altitude, double distance, double bearing, double closure_rate, double time_to_impact, uint8_t threat_level);
void get_buffer_snapshot(SimulationState* retSimWorld);

#endif