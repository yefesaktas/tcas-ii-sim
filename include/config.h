#ifndef CONFIG_H
#define CONFIG_H

// Thread timings

/**
 * @brief Period between display updates (microseconds).
 *
 * The display thread sleeps this long between each
 * render cycle.
 */
#define DISPLAY_REFRESH_RATE_US 33000L // ~30 fps

/**
 * @brief Period between transponder updates (microseconds).
 *
 * The simulated transponder thread sleeps this long between each
 * broadcast/update cycle.
 */
#define TRANSPONDER_UPDATE_PERIOD_US 500000L

/**
 * @brief Period between TCAS updates (microseconds).
 *
 * The simulated tcas logic thread sleeps this long between each
 * update cycle.
 */
#define TCAS_UPDATE_PERIOD_US 1000000L

// Simulation parameters

/**
 * @brief Maximum number of intruder tracks supported by the simulation.
 *
 * @details TCAS II version 7.1 can track up to 30 transponder-equipped
 * aircraft simultaneously; this constant bounds the intruder array sizes
 * used throughout the simulator.
 */
#define MAX_TRACK 30 

/**
 * @brief Number of intruder aircraft simulated.
 *
 * Set to 1 for the simple test scenario; real simulations may use
 * multiple intruders up to `MAX_TRACK`.
 */
#define INTRUDERS_NUM 1

/**
 * @brief TAU threshold in seconds for triggering a resolution advisory.
 */
#define TAU_RA_THRESHOLD 25.0 

/**
 * @brief TAU threshold in seconds for triggering a traffic advisory.
 */
#define TAU_TA_THRESHOLD 40.0

#endif