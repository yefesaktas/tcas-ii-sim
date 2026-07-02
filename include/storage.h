/**
 * @file storage.h
 * @brief Thread-safe shared buffer API for the simulation state.
 *
 * Declares initialization, update and snapshot functions used by
 * producer and consumer threads to exchange simulation state.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 *
 * \msc
 * Producer, Storage, Consumer;
 * Producer->Storage [label="set_* / update_intruder_data()"];
 * Storage->Consumer [label="get_buffer_snapshot()"];
 * \endmsc
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <signal.h>

#include "types.h"

/**
 * @brief Flag set when a SIGINT (Ctrl+C) has been observed.
 *
 * This signal-safe atomic flag is used by threads to cooperatively detect a
 * shutdown request and exit cleanly.
 */
extern volatile sig_atomic_t isShutdownSignaled;

// function prototypes

/**
 * @brief Initialize internal buffers and synchronization primitives.
 *
 * @details Must be called before any producer or consumer threads access the
 * shared simulation state.
 *
 * \msc
 * Caller, Storage;
 * Caller->Storage [label="init_buffer()"];
 * \endmsc
 */
void init_buffer(void);

/**
 * @brief Finalize and release storage resources.
 *
 * @details Should be called during program shutdown after threads have joined.
 *
 * \msc
 * Caller, Storage;
 * Caller->Storage [label="finalize_buffer()"];
 * \endmsc
 */
void finalize_buffer(void);

/**
 * @brief 
 *
 * @details 
 * 
 * @param
 *
 * \msc
 * 
 * \endmsc
 */
void set_SimWorld_state(const AircraftState* host_state, uint32_t host_mode_s_code,
                        const AircraftState* intruders_state, const uint32_t* intruders_mode_s,
                        int num_intruders);

/**
 * @brief Store computed relative metrics for an intruder track.
 *
 * @details 
 *
 * @param 
 *
 * \msc
 * 
 * \endmsc
 */
void update_intruders_tcas_data(const TCAS_Metrics* computed_metrics, int num_intruders);

/**
 * @brief Obtain a thread-safe snapshot of the simulation state.
 *
 * @details Copies the internal `SimulationState` into the caller-provided
 * `retSimWorld` buffer. The caller is responsible for providing a
 * valid pointer to a `SimulationState` sized object.
 *
 * @param retSimWorld Pointer to a `SimulationState` structure to receive
 * the snapshot copy.
 *
 * \msc
 * Consumer, Storage;
 * Consumer->Storage [label="get_buffer_snapshot(retSimWorld)"];
 * Storage->Consumer [label="copy SimulationState -> retSimWorld"];
 * \endmsc
 */
void get_buffer_snapshot(SimulationState* retSimWorld);

#endif