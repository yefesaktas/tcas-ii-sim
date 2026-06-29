/**
 * @file transponder_data.h
 * @brief Transponder data producer thread interface.
 *
 * Declares the entrypoint for the transponder simulation thread which
 * periodically updates the shared simulation buffer.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 *
 * \msc
 * TransponderThread, Storage;
 * TransponderThread->Storage [label="set_OwnShip_state() / set_IntruderShip_state() calls"];
 * \endmsc
 */

#ifndef TRANSPONDER_DATA_H
#define TRANSPONDER_DATA_H

/**
 * @brief Transponder data producer thread.
 *
 * @details This thread simulates periodic transponder broadcasts for the host
 * and intruder aircrafts, updating the shared simulation buffer via the
 * `storage` API until `isShutdownSignaled` becomes true.
 *
 * @param arg Optional thread argument (unused, may be NULL).
 * @return void* Always returns NULL when the thread exits normally.
 *
 * \msc
 * TransponderThread, Storage;
 * TransponderThread->Storage [label="set_OwnShip_state(&host_state, host_ModeS)"];
 * TransponderThread->Storage [label="set_IntruderShip_state(i, &intruder_state[i], id)"];
 * \endmsc
 */
void* transponder_data_thread(void* arg);

#endif