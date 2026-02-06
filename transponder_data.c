#include "storage.h"
#include <unistd.h>

#define UPDATE_PERIOD_US 500000 // 0.5 seconds (in microseconds)
#define DELTA_TIME 0.5
#define INTRUDERS_NUM 1 // TEST

static AircraftState host_state;
static uint32_t host_ModeS;

static AircraftState intruder_state[INTRUDERS_NUM];
static uint32_t intruder_ModeS[INTRUDERS_NUM];

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

    while (!SIGINT_signaled){
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
