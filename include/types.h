/**
 * @file types.h
 * @brief Type definitions and shared data structures for the TCAS II simulator.
 *
 * This header defines enums, structs and constants used across the
 * simulator (ownship, intruder tracks, and the global simulation state).
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 *
 * \msc
 * Producer, SimulationState, Consumer;
 * Producer->SimulationState [label="write state / update values"];
 * SimulationState->Consumer [label="get_buffer_snapshot()"];
 * \endmsc
 */

#ifndef TYPES_H
#define TYPES_H

/**
 * @brief Maximum number of intruder tracks supported by the simulation.
 *
 * @details TCAS II version 7.1 can track up to 30 transponder-equipped
 * aircraft simultaneously; this constant bounds the intruder array sizes
 * used throughout the simulator.
 */
#define MAX_TRACK 30 

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Threat level classification used by the TCAS logic. */
typedef enum {
    THREAT_NONE = 0, /**< No advisory required. */
    THREAT_TA   = 1, /**< Traffic advisory (TA): traffic nearby but no RA. */
    THREAT_RA   = 2  /**< Resolution advisory (RA): evasive maneuver required. */
} ThreatLevel;

/**
 * @brief Aircraft 3D position and velocity.
 *
 * @details Positions are in meters in an Earth-relative Cartesian frame
 * and velocities are in meters/second. `z` and `vz` represent altitude and
 * vertical speed respectively.
 */
typedef struct {
    double x, y, z;           /**< Position in meters (Cartesian frame). */
    double vx, vy, vz;        /**< Velocity in meters/second. `vz` is vertical speed. */
} AircraftState;

/**
 * @brief Host (own) aircraft identifier and state.
 *
 * @details Contains the Mode-S identifier (`mode_s_code`) and the current
 * `AircraftState` for the ownship.
 */
typedef struct {    
    uint32_t mode_s_code;     /**< Mode-S transponder code (24-bit identifier). */

    AircraftState state;      /**< Current ownship kinematic state. */
} OwnShip; 

/**
 * @brief Data describing a tracked intruder aircraft.
 *
 * @details `isActive` indicates whether this array slot currently holds a
 * valid intruder. Positional and relative metrics are provided alongside the
 * intruder's full `AircraftState`. `threat_level` stores a value from
 * `ThreatLevel`.
 */
typedef struct {
    bool isActive;            /**< true if this slot holds a valid intruder. */
    uint32_t mode_s_code;     /**< Mode-S transponder code for the intruder. */

    double relative_altitude; /**< meters: intruder altitude - ownship altitude. */
    double distance;          /**< meters: slant range from ownship. */
    double bearing;           /**< radians: bearing from ownship to intruder. */
    double closure_rate;      /**< meters/second: positive when closing. */
    double time_to_impact;    /**< seconds: estimated time to closest approach (may be INFINITY). */

    AircraftState state;      /**< Kinematic state of the intruder. */
    uint8_t threat_level;     /**< Encoded `ThreatLevel` value (uses `uint8_t` for compactness). */
} IntruderShip;

/**
 * @brief Global simulation state container.
 *
 * @details Holds the current `OwnShip` data, an array of tracked
 * `IntruderShip` entries (size `MAX_TRACK`), and a mutex
 * (`state_access_lock`) used to synchronize concurrent access between
 * producer and consumer threads.
 */
typedef struct {
    OwnShip host_aircraft;                    /**< Data for the ownship. */
    IntruderShip intruders[MAX_TRACK];        /**< Array of tracked intruders. */
    pthread_mutex_t state_access_lock;        /**< Mutex to synchronize concurrent access. */
} SimulationState;

#endif