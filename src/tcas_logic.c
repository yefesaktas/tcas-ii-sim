/**
 * @file tcas_logic.c
 * @brief TCAS advisory computation worker.
 *
 * Implements the thread that snapshots simulation state, computes intruder
 * metrics, and publishes the advisory results.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include "tcas_logic.h"

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "config.h"
#include "types.h"
#include "storage.h"

static TCAS_Metrics local_metrics[MAX_TRACK] = {0};
static SimulationState tcas_simWorld_buffer;

/**
 * @brief Snapshot the current simulation state for TCAS processing.
 *
 * \msc
 * TCASLogicThread, Storage;
 * TCASLogicThread->Storage [label="get_buffer_snapshot()"];
 * \endmsc
 */
static void fetch_tcas_data(){
    get_buffer_snapshot(&tcas_simWorld_buffer);
} // fetch_tcas_data end

/**
 * @brief Compute per-intruder relative range, closure, and threat metrics.
 *
 * \msc
 * TCASLogicThread, TCASMetrics;
 * TCASLogicThread->TCASMetrics [label="compute_tcas_metrics()"];
 * \endmsc
 */
static void compute_tcas_metrics(){
    AircraftState host_state = tcas_simWorld_buffer.host_aircraft.state;

    for (int i = 0; i < INTRUDERS_NUM; i++){
        if (!tcas_simWorld_buffer.intruders[i].isActive){
            continue;
        }

        AircraftState intruder_state = tcas_simWorld_buffer.intruders[i].state;

        // calculate relative-to-host position vector
        double dx = intruder_state.x - host_state.x;
        double dy = intruder_state.y - host_state.y;
        double dz = intruder_state.z - host_state.z;

        // calculate relative-to-host velocity vector
        double dvx = intruder_state.vx - host_state.vx;
        double dvy = intruder_state.vy - host_state.vy;
        double dvz = intruder_state.vz - host_state.vz;

        // store relative-to-host altitude in local array
        local_metrics[i].relative_altitude = dz;

        // calculate and store slant range
        double distance = sqrt(dx*dx + dy*dy + dz*dz);
        local_metrics[i].distance = distance;

        // calculate and store bearing
        local_metrics[i].bearing = atan2(dy, dx);

        // calculate and store closure rate
        double closure_rate = 0.0;

        if (distance > 0.001){ // to prevent potential Division by Zero error
            closure_rate = -((dx * dvx) + (dy * dvy) + (dz * dvz)) / distance;
        }

        local_metrics[i].closure_rate = closure_rate;

        // calculate and store time to impact (tau) if closure_rate is positive
        if (closure_rate > 0.0){
            local_metrics[i].time_to_impact = distance / closure_rate;
        }

        else{
            local_metrics[i].time_to_impact = INFINITY; // no collision risk
        }

        // identify level (threat logic)
        if (local_metrics[i].time_to_impact <= TAU_RA_THRESHOLD){ 
            local_metrics[i].threat_level = THREAT_RA;
        } 

        else if (local_metrics[i].time_to_impact <= TAU_TA_THRESHOLD){
            local_metrics[i].threat_level = THREAT_TA;
        } 

        else{
            local_metrics[i].threat_level = THREAT_NONE;
        }
    } // for end
} // compute_tcas_metrics end

void* tcas_logic_thread(void* arg){ // thread function
    printf("[TCAS LOGIC THREAD] AVAIL\n");

    struct timespec start_time, end_time; // used for dynamic sleep

    while (!isShutdownSignaled){
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        fetch_tcas_data();

        compute_tcas_metrics();

        // store all the computed data in the shared buffer
        update_intruders_tcas_data(local_metrics, INTRUDERS_NUM);

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // calculate duration of operations in microseconds
        long work_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L + 
                            (end_time.tv_nsec - start_time.tv_nsec) / 1000L;

        // calculate remaining sleep time in microseconds
        long sleep_time_us = TCAS_UPDATE_PERIOD_US - work_time_us;

        // sleep if needed
        if (sleep_time_us > 0){
            usleep(sleep_time_us);
        }
    } // while end

    printf("[TCAS LOGIC THREAD] Shutdown Successful\n");

    return NULL;
} // tcas_logic_thread end