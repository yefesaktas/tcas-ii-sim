/**
 * @file transponder_data.c
 * @brief Simulate periodic transponder broadcasts for host and intruders.
 *
 * Implements the transponder producer thread routine that updates the
 * shared `SimulationState` via the storage API.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include "transponder_data.h"

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "config.h"
#include "types.h"
#include "storage.h"

static AircraftState host_state;
static uint32_t host_ModeS;

static AircraftState intruder_state[INTRUDERS_NUM];
static uint32_t intruder_ModeS[INTRUDERS_NUM];

/**
 * @brief Initialize a simple scenario for host and intruder(s).
 *
 * Sets initial positions, velocities and Mode-S identifiers used by the
 * simulated transponder updates.
 *
 * \msc
 * TransponderThread, Init;
 * TransponderThread->Init [label="init_scenerio(): set host_state / intruder_state"];
 * \endmsc
 */
static void init_scenerio(){
    // initial coordinate and mode-s inputs

    // host aircraft
    host_ModeS = 0xABC123;

    // position data
    host_state.x = 0;
    host_state.y = 0;
    host_state.z = 1000;

    // velocity vector data
    host_state.vx = 0;
    host_state.vy = 100;
    host_state.vz = 0;

    // intruder 1
    intruder_ModeS[0] = 0x000000;

    // position data
    intruder_state[0].x = 0;
    intruder_state[0].y = 4000;
    intruder_state[0].z = 1000;

    // velocity vector data
    intruder_state[0].vx = 0;
    intruder_state[0].vy = -100;
    intruder_state[0].vz = 0;
} // init_scenerio end

// stores the last calculation time of the physics loop
static struct timespec last_physics_time = {0};

/**
 * @brief Advance the simulated aircraft states by one timestep.
 *
 * Applies a simple constant-velocity integration using a dynamically 
 * calculated delta time (real-time elapsed since the last update).
 *
 * \msc
 * TransponderThread, Physics;
 * TransponderThread->Physics [label="update_physics(): advance positions"];
 * Physics->TransponderThread [label="updated host_state / intruder_state"];
 * \endmsc
 */
static void update_physics(){
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    // skip the first run to establish a baseline time
    if (last_physics_time.tv_sec == 0 && last_physics_time.tv_nsec == 0) {
        last_physics_time = current_time;
        return; 
    }

    // calculate dynamic delta_t in seconds
    double delta_t = (current_time.tv_sec - last_physics_time.tv_sec) + 
                     (current_time.tv_nsec - last_physics_time.tv_nsec) / 1e9;

    last_physics_time = current_time;

    // update host aircraft
    host_state.x += host_state.vx * delta_t;
    host_state.y += host_state.vy * delta_t;
    host_state.z += host_state.vz * delta_t;

    // update intruder aircrafts
    for (int i = 0; i < INTRUDERS_NUM; i++){
        intruder_state[i].x += intruder_state[i].vx * delta_t;
        intruder_state[i].y += intruder_state[i].vy * delta_t;
        intruder_state[i].z += intruder_state[i].vz * delta_t;
    }
} // update_physics end

void* transponder_data_thread(void* arg){ // thread function
    init_scenerio();

    printf("[TRANSPONDER THREAD] AVAIL\n");

    struct timespec start_time, end_time; // used for dynamic sleep

    while (!isShutdownSignaled){
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        update_physics();

        set_SimWorld_state(&host_state, host_ModeS,
                            intruder_state, intruder_ModeS,
                            INTRUDERS_NUM);

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // calculate duration of operations in microseconds
        long work_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L + 
                            (end_time.tv_nsec - start_time.tv_nsec) / 1000L;

        // calculate remaining sleep time in microseconds
        long sleep_time_us = TRANSPONDER_UPDATE_PERIOD_US - work_time_us;

        // sleep if needed
        if (sleep_time_us > 0){
            usleep(sleep_time_us);
        }
    } // while end

    printf("[TRANSPONDER THREAD] Shutdown Successful\n");

    return NULL;
} // transponder_data_thread end
