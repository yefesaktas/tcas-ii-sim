#include "storage.h"
#include <string.h>

volatile atomic_bool SIGINT_signaled = 0;
static SimulationState simWorld; // prevent access from outside with extern keyword by using static keyword

void init_buffer(void){
    memset(&simWorld, 0, sizeof(simWorld)); // clear buffer

    pthread_mutex_init(&(simWorld.state_access_lock), NULL); // initialize shared buffer lock
} // init_buffer end

void finalize_buffer(void){
    pthread_mutex_destroy(&(simWorld.state_access_lock)); // destroy shared buffer lock
} // finalize_buffer end

void set_OwnShip_state(const AircraftState* state, uint32_t mode_s_code){ // transponder_data thread will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    memcpy(&(simWorld.host_aircraft.state), state, sizeof(simWorld.host_aircraft.state)); // set aircraft state
    simWorld.host_aircraft.mode_s_code = mode_s_code; // set mode-S code

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // set_OwnShip_state end

void set_IntruderShip_state(int index, const AircraftState* state, uint32_t mode_s_code){ // transponder_data thread will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    memcpy(&(simWorld.intruders[index].state), state, sizeof(simWorld.intruders[index].state)); // set aircraft state
    simWorld.intruders[index].mode_s_code = mode_s_code; // set mode-S code
    simWorld.intruders[index].isActive = true;

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // set_IntruderShip_state end

void update_intruder_data(int index, double relative_altitude, double distance, double bearing, double closure_rate, double time_to_impact, uint8_t threat_level){ // tcas_logic thread will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    simWorld.intruders[index].relative_altitude = relative_altitude;
    simWorld.intruders[index].distance = distance;
    simWorld.intruders[index].bearing = bearing;
    simWorld.intruders[index].closure_rate = closure_rate;
    simWorld.intruders[index].time_to_impact = time_to_impact;
    simWorld.intruders[index].threat_level = threat_level;

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // update_intruder_data end

void get_buffer_snapshot(SimulationState* retSimWorld){ // tcas_logic and display threads will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    memcpy(retSimWorld, &simWorld, sizeof(simWorld)); // copy the current simulation state

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // get_buffer_snapshot end

