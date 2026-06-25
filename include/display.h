/**
 * @file display.h
 * @brief Display consumer thread interface.
 *
 * Declares the entrypoint for the user interface display thread which
 * periodically fetches the shared simulation buffer and then renders on terminal (TUI).
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 *
 * \msc
 * 
 * \endmsc
 */

#ifndef DISPLAY_H
#define DISPLAY_H

/**
 * @brief Display consumer thread.
 *
 * @details
 * 
 * @param arg Optional thread argument (unused, may be NULL).
 * @return void* Always returns NULL when the thread exits normally.
 *
 * \msc
 * 
 * \endmsc
 */
void* display_thread(void* arg);

#endif