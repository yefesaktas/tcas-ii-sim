/**
 * @file storage.h
 * @brief Thread-safe shared buffer API for the simulation state.
 *
 * Declares initialization, update and snapshot functions used by
 * producer and consumer threads to exchange simulation state.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdatomic.h>

#include "types.h"

/**
 * @brief Flag set when a SIGINT (Ctrl+C) has been observed.
 *
 * This atomic boolean is used by threads to cooperatively detect a
 * shutdown request and exit cleanly.
 */
extern volatile atomic_bool isSIGINT_signaled;

// function prototypes

/**
 * @brief Initialize internal buffers and synchronization primitives.
 *
 * Must be called before any producer or consumer threads access the
 * shared simulation state.
 */
void init_buffer(void);

/**
 * @brief Tear down buffers and release resources allocated by
 * `init_buffer`.
 *
 * Should be called during program shutdown after threads have joined.
 */
void finalize_buffer(void);

/**
 * @brief Update the stored ownship state.
 *
 * Copies the provided `state` and `mode_s_code` into the global
 * simulation buffer in a thread-safe manner.
 *
 * @param state Pointer to the new `AircraftState` for the ownship.
 * @param mode_s_code Mode-S identifier associated with the ownship.
 */
void set_OwnShip_state(const AircraftState* state, uint32_t mode_s_code);

/**
 * @brief Update the positional `AircraftState` and Mode-S for an intruder.
 *
 * Marks the intruder slot at `index` as active and updates its state.
 * The function is thread-safe with respect to the shared simulation
 * buffer.
 *
 * @param index Index into the intruder array (0..MAX_TRACK-1).
 * @param state Pointer to the new `AircraftState` for the intruder.
 * @param mode_s_code Mode-S identifier for the intruder aircraft.
 */
void set_IntruderShip_state(int index, const AircraftState* state, uint32_t mode_s_code);

/**
 * @brief Update computed relative metrics for an intruder track.
 *
 * These values (relative altitude, distance, bearing, closure rate,
 * time to impact and threat level) are typically computed by the
 * TCAS logic and stored for display and audio modules.
 *
 * @param index Index into the intruder array (0..MAX_TRACK-1).
 * @param relative_altitude Altitude difference relative to ownship (meters).
 * @param distance Slant range distance to the intruder (meters).
 * @param bearing Bearing from ownship to intruder (radians).
 * @param closure_rate Rate at which separation is changing (m/s).
 * @param time_to_impact Estimated time until closest approach (seconds).
 * @param threat_level Threat classification (value from `ThreatLevel`).
 */
void update_intruder_data(int index, double relative_altitude, double distance, double bearing, double closure_rate, double time_to_impact, uint8_t threat_level);

/**
 * @brief Obtain a thread-safe snapshot of the current simulation state.
 *
 * Copies the internal `SimulationState` into the caller-provided
 * `retSimWorld` buffer. The caller is responsible for providing a
 * valid pointer to a `SimulationState` sized object.
 *
 * @param retSimWorld Pointer to a `SimulationState` structure to receive
 * the snapshot copy.
 */
void get_buffer_snapshot(SimulationState* retSimWorld);

#endif