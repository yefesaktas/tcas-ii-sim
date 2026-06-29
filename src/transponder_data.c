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
#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>

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
 * @brief Simulation timestep in seconds used to advance simple physics.
 */
#define DELTA_TIME 0.5

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

/**
 * @brief Advance the simulated aircraft states by one timestep.
 *
 * Applies a simple constant-velocity integration using `DELTA_TIME`.
 *
 * \msc
 * TransponderThread, Physics;
 * TransponderThread->Physics [label="update_physics(): advance positions"];
 * Physics->TransponderThread [label="updated host_state / intruder_state"];
 * \endmsc
 */
static void update_physics(){
    // update host aircraft data
    host_state.x += host_state.vx * DELTA_TIME;
    host_state.y += host_state.vy * DELTA_TIME;
    host_state.z += host_state.vz * DELTA_TIME;

    // update intruder aircrafts data
    for (int i = 0; i < INTRUDERS_NUM; i++){
        intruder_state[i].x += intruder_state[i].vx * DELTA_TIME;
        intruder_state[i].y += intruder_state[i].vy * DELTA_TIME;
        intruder_state[i].z += intruder_state[i].vz * DELTA_TIME;
    } // for end
} // update_physics end

void* transponder_data_thread(void* arg){ // thread function
    init_scenerio();

    printf("[TRANSPONDER THREAD] AVAIL\n");

    while (!atomic_load(&isShutdownSignaled)){
        update_physics();

        set_OwnShip_state(&host_state, host_ModeS);

        for (int i = 0; i < INTRUDERS_NUM; i++){
            set_IntruderShip_state(i, &intruder_state[i], intruder_ModeS[i]);
        } // for end

        usleep(UPDATE_PERIOD_US); // sleep 0.5 second
    } // while end

    printf("[TRANSPONDER THREAD] Shutdown Successful\n");

    return NULL;
} // transponder_data_thread end
