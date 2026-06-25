/**
 * @file display.c
 * @brief Textual User Interface (TUI) based on ncurses library
 *
 * Initializes the ncurses TUI display, copies simulation state from shared buffer in a thread-safe manner,
 * renders the display, frees resources and resets the terminal on exit.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include "display.h"

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "types.h"
#include "storage.h"

static WINDOW* radar_win;
static WINDOW* telemetry_win;
static WINDOW* status_win;
static SimulationState* simWorld_buffer;

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void init_display(){
    setlocale(LC_ALL, "");

    initscr();
    cbreak(); // catch POSIX signals
    noecho();
    curs_set(0); // hide flashing terminal cursor

    // initialize color support if terminal supports
    if (has_colors()) {
        start_color();

        // init_pair(pair_id, foreground_color, background_color)
        init_pair(1, COLOR_RED, COLOR_BLACK);    // for RA (Resolution Advisory)
        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // for TA (Traffic Advisory)
        init_pair(3, COLOR_CYAN, COLOR_BLACK);   // for Proximate/Other Traffic
        init_pair(4, COLOR_GREEN, COLOR_BLACK);  // for Safe Status / Grid lines
    } // if end

    // width and height variables
    int radar_width = COLS * 0.7;           // 70% of display
    int right_width = COLS - radar_width;   // remaining 30%
    int status_height = 3;                  // 3 lines for status window height

    // create display sections (windows) in memory
    radar_win = newwin(LINES, radar_width, 0, 0);
    telemetry_win = newwin(LINES - status_height, right_width, 0, radar_width);
    status_win = newwin(status_height, right_width, LINES - status_height, radar_width);

} // init_screen end

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void destroy_display(){

} // destroy_display end

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void fetch_display_data(){

} // fetch_display_data end

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void render_radar(){

} // render_radar end

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void render_telemetry(){

} // render_telemetry end

/**
 * @brief 
 *
 * \msc
 *
 * \endmsc
 */
static void render_status(){

} // render_status end

void* display_thread(void* arg){ // thread function

} // display_thread end