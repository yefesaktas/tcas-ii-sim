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

#include "types.h"
#include "storage.h"

/**
 * @brief Period between transponder updates (microseconds).
 *
 * The simulated transponder thread sleeps this long between each
 * broadcast/update cycle.
 */
#define UPDATE_PERIOD_US 500000

/**
 * @brief Number of intruder aircraft simulated.
 *
 * Set to 1 for the simple test scenario; real simulations may use
 * multiple intruders up to `MAX_TRACK`.
 */
#define INTRUDERS_NUM 1

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

    while (!isShutdownSignaled){
        update_physics();

        set_SimWorld_state(&host_state, host_ModeS,
                            intruder_state, intruder_ModeS,
                            INTRUDERS_NUM);

        usleep(UPDATE_PERIOD_US); // sleep 0.5 second
    } // while end

    printf("[TRANSPONDER THREAD] Shutdown Successful\n");

    return NULL;
} // transponder_data_thread end
