/**
 * @file transponder_data.h
 * @brief Transponder data producer thread interface.
 *
 * Declares the entrypoint for the transponder simulation thread which
 * periodically updates the shared simulation buffer.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#ifndef TRANSPONDER_DATA_H
#define TRANSPONDER_DATA_H

/**
 * @brief Entry point for the transponder data producer thread.
 *
 * This thread simulates periodic transponder broadcasts for the host
 * and intruder aircrafts, updating the shared simulation buffer via the
 * `storage` API until `isSIGINT_signaled` becomes true.
 *
 * @param arg Optional thread argument (unused, may be NULL).
 * @return void* Always returns NULL when the thread exits normally.
 */
void* transponder_data_thread(void* arg);

#endif