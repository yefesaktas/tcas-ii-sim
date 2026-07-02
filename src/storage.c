/**
 * @file storage.c
 * @brief Thread-safe storage implementation for the simulation state.
 *
 * Implements buffer initialization, update helpers and snapshot accessors
 * used by producer and consumer threads.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include "storage.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include "types.h"

volatile sig_atomic_t isShutdownSignaled = 0;
static SimulationState simWorld; // prevent access from outside with extern keyword by using static keyword

void init_buffer(void){
    memset(&simWorld, 0, sizeof(simWorld)); // clear buffer

    pthread_mutex_init(&(simWorld.state_access_lock), NULL); // initialize shared buffer lock
} // init_buffer end

void finalize_buffer(void){
    pthread_mutex_destroy(&(simWorld.state_access_lock)); // destroy shared buffer lock
} // finalize_buffer end

void set_SimWorld_state(const AircraftState* host_state, uint32_t host_mode_s_code, // transponder_data thread will call
                        const AircraftState* intruders_state, const uint32_t* intruders_mode_s,
                        int num_intruders){

    pthread_mutex_lock(&(simWorld.state_access_lock));

    // update ownship state
    memcpy(&(simWorld.host_aircraft.state), host_state, sizeof(simWorld.host_aircraft.state));
    simWorld.host_aircraft.mode_s_code = host_mode_s_code;

    // update intruder states
    for (int i = 0; i < num_intruders; i++){
        memcpy(&(simWorld.intruders[i].state), &intruders_state[i], sizeof(simWorld.intruders[i].state));
        simWorld.intruders[i].mode_s_code = intruders_mode_s[i]; // set mode-S code
        simWorld.intruders[i].isActive = true;
    }

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // set_SimWorld_state

void update_intruders_tcas_data(const TCAS_Metrics* computed_metrics, int num_intruders){ // tcas_logic thread will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    for (int i = 0; i < num_intruders; i++){
        simWorld.intruders[i].metrics = computed_metrics[i];
    }
    
    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // update_intruder_data end

void get_buffer_snapshot(SimulationState* retSimWorld){ // tcas_logic and display threads will call
    pthread_mutex_lock(&(simWorld.state_access_lock));

    memcpy(retSimWorld, &simWorld, sizeof(simWorld)); // copy the current simulation state

    pthread_mutex_unlock(&(simWorld.state_access_lock));
} // get_buffer_snapshot end

