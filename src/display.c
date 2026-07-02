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
#include <math.h>
#include <time.h>

#include "types.h"
#include "storage.h"

#define PI 3.14159265

/**
 * @brief Period between display updates (microseconds).
 *
 * The display thread sleeps this long between each
 * render cycle.
 */
#define DISPLAY_REFRESH_RATE_US 33000 // ~30 fps

static WINDOW* radar_win;
static WINDOW* telemetry_win;
static WINDOW* status_win;
static SimulationState display_simWorld_buffer;

/**
 * @brief Initialize ncurses display with windows and color pairs.
 */
static void init_display(){
    setlocale(LC_ALL, "");

    initscr();
    cbreak(); // catch POSIX signals
    noecho();
    curs_set(0); // hide flashing terminal cursor

    // initialize color support if terminal supports
    if (has_colors()){
        start_color();

        // init_pair(pair_id, foreground_color, background_color)
        init_pair(1, COLOR_RED, COLOR_BLACK);    // for RA (Resolution Advisory)
        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // for TA (Traffic Advisory)
        init_pair(3, COLOR_CYAN, COLOR_BLACK);   // for Proximate/Other Traffic
        init_pair(4, COLOR_GREEN, COLOR_BLACK);  // for Safe Status / Grid lines
        init_pair(5, COLOR_WHITE, COLOR_BLACK);  // background
    }

    // width and height variables
    int radar_width = COLS * 0.7;           // 70% of display
    int right_width = COLS - radar_width;   // remaining 30%
    int status_height = 6;                  // 3 lines for status window height

    // create display sections (windows) in memory
    radar_win = newwin(LINES, radar_width, 0, 0);
    telemetry_win = newwin(LINES - status_height, right_width, 0, radar_width);
    status_win = newwin(status_height, right_width, LINES - status_height, radar_width);

    // set window background colors
    wbkgd(radar_win, COLOR_PAIR(5));
    wbkgd(telemetry_win, COLOR_PAIR(5));
    wbkgd(status_win, COLOR_PAIR(5));
} // init_screen end

/**
 * @brief Clean up and finalize ncurses display resources.
 */
static void destroy_display(){
    // deallocate window memories
    if (radar_win != NULL){
        delwin(radar_win);
    }

    if (telemetry_win != NULL){
        delwin(telemetry_win);
    }

    if (status_win != NULL){
        delwin(status_win);
    }

    endwin(); // exit ncurses textual ui mode
} // destroy_display end

/**
 * @brief Fetch current simulation state from shared buffer.
 */
static void fetch_display_data(){
    get_buffer_snapshot(&display_simWorld_buffer);
} // fetch_display_data end

/**
 * @brief Draw radar rings, center marker, and window border.
 *
 * @param max_y Radar window height.
 * @param max_x Radar window width.
 * @param center_y Y coordinate of the radar center.
 * @param center_x X coordinate of the radar center.
 */
static void draw_radar_background(int max_y, int max_x, int center_y, int center_x){
    // draw range rings
    int num_rings = 3;
    int max_radius = (max_y / 2) - 2;
    int ring_step = max_radius / num_rings;

    wattron(radar_win, COLOR_PAIR(3)); // activate ring colors (cyan)

    for (int r = ring_step; r <= max_radius; r += ring_step) {
        // rotate 360 degrees around the ring with intervals of 10 degrees
        for (int angle = 0; angle < 360; angle += 10) {
            double rad = angle * (PI / 180.0); // deg to rad

            // scale x-axis by 2 (to fit characters into terminal properly)
            int y = (int)(r * sin(rad));
            int x = (int)(r * 2.0 * cos(rad)); 

            // perform window boundary check and print
            if (center_y + y > 0 && center_y + y < max_y - 1 &&
                center_x + x > 0 && center_x + x < max_x - 1) {
                mvwaddch(radar_win, center_y + y, center_x + x, '.');
            }
        } // for end
    } // for end

    wattroff(radar_win, COLOR_PAIR(3)); // deactivate ring colors

    // draw host aircraft on center in green
    wattron(radar_win, COLOR_PAIR(4));
    mvwaddch(radar_win, center_y, center_x, '^');
    wattroff(radar_win, COLOR_PAIR(4));

    box(radar_win, 0, 0); // draw window border
    mvwprintw(radar_win, 0, 2, " TCAS NAVIGATION DISPLAY "); // add window header
} // draw_radar_background end

/**
 * @brief Convert raw host/intruder positions into radar screen offsets.
 *
 * @details Computes the intruder position relative to the host aircraft and
 * scales it into radar-character offsets suitable for drawing on the ncurses
 * radar window.
 *
 * @param host_state Current ownship state.
 * @param intruder_state Current intruder state.
 * @param max_y Radar window height.
 * @param max_x Radar window width.
 * @param offset_y Output row offset from the radar center (center_y).
 * @param offset_x Output column offset from the radar center (center_x).
 */
static void compute_intruder_screen_offset(const AircraftState* host_state, const AircraftState* intruder_state, int max_y, int max_x, int* offset_y, int* offset_x){
    double relative_x = intruder_state->x - host_state->x;
    double relative_y = intruder_state->y - host_state->y;

    int max_radius = (max_y / 2) - 2;

    /* maximum radar range: 30 nautical miles -> meters (1 NM = 1852 m) */
    double max_range = 30.0 * 1852.0; // = 55560 meters 
    double range_scale = (double)max_radius / max_range;

    int scaled_y = (int)lround(relative_y * range_scale);
    int scaled_x = (int)lround(relative_x * range_scale * 2.0);

    if (scaled_y > max_radius){
        scaled_y = max_radius;
    } 
    
    else if (scaled_y < -max_radius){
        scaled_y = -max_radius;
    }

    if (scaled_x > (max_x / 2) - 2){
        scaled_x = (max_x / 2) - 2;
    } 
    
    else if (scaled_x < -((max_x / 2) - 2)){
        scaled_x = -((max_x / 2) - 2);
    }

    *offset_y = -scaled_y;
    *offset_x = scaled_x;
} // compute_intruder_screen_offset end

/**
 * @brief Draw active intruder aircraft symbols on radar display.
 *
 * @param max_y Radar window height.
 * @param max_x Radar window width.
 * @param center_y Y coordinate of the radar center.
 * @param center_x X coordinate of the radar center.
 */
static void draw_intruders(int max_y, int max_x, int center_y, int center_x){
    // iterate tracked intruders and draw each active one
    for (int i = 0; i < MAX_TRACK; ++i){
        const IntruderShip* intr = &display_simWorld_buffer.intruders[i];

        if (!intr->isActive){
            continue;
        } 
 
        int off_y = 0, off_x = 0;
        compute_intruder_screen_offset(&display_simWorld_buffer.host_aircraft.state, &intr->state, max_y, max_x, &off_y, &off_x);

        int draw_y = center_y + off_y;
        int draw_x = center_x + off_x;

        if (draw_y <= 0 || draw_y >= max_y - 1 || draw_x <= 0 || draw_x >= max_x - 1){
            continue; // out of drawable area
        } 

        // choose symbol and color by threat level
        char ch = 'O';
        int color_pair = 3; // default: proximate/other (cyan)

        switch (intr->metrics.threat_level){
            case THREAT_RA: // resolution advisory
                ch = 'R';
                color_pair = 1; // red
                break;

            case THREAT_TA: // traffic advisory
                ch = 'T';
                color_pair = 2; // yellow
                break;

            default:
                break; // keep defaults
        } // switch-case end

        wattron(radar_win, COLOR_PAIR(color_pair));
        mvwaddch(radar_win, draw_y, draw_x, ch);
        wattroff(radar_win, COLOR_PAIR(color_pair));
    } // for end
} // draw_intruders end

/**
 * @brief Render complete radar display with background and intruders.
 */
static void render_radar(){
    werase(radar_win); // erase previous window frame

    int max_y, max_x;
    getmaxyx(radar_win, max_y, max_x); // fetch radar window height (max_y) and width (max_x)

    // calculate center coordinates for host aircraft
    int center_y = max_y / 2;
    int center_x = max_x / 2;

    draw_radar_background(max_y, max_x, center_y, center_x);
    draw_intruders(max_y, max_x, center_y, center_x);

    wnoutrefresh(radar_win); // queue window changes in memory
} // render_radar end

/**
 * @brief Render telemetry and active track information panel.
 */
static void render_telemetry(){
    werase(telemetry_win); // erase previous window frame

    int max_y, max_x;
    getmaxyx(telemetry_win, max_y, max_x); // fetch telemetry window height (max_y) and width (max_x)

    box(telemetry_win, 0, 0); // draw window border
    mvwprintw(telemetry_win, 0, 2, " TELEMETRY & TRACKING "); // add window header

    // host aircraft telemetry data
    const AircraftState* host = &display_simWorld_buffer.host_aircraft.state;

    wattron(telemetry_win, A_BOLD | COLOR_PAIR(4)); // bold and green
    mvwprintw(telemetry_win, 2, 2, "OWN AIRCRAFT (HOST)");
    wattroff(telemetry_win, A_BOLD | COLOR_PAIR(4));

    // add altitude and position data (add mode_s in future)
    double host_alt_ft = host->z * 3.28084; // meter to feet conversion

    mvwprintw(telemetry_win, 4, 2, "ALTITUDE : %.0f ft", host_alt_ft);
    mvwprintw(telemetry_win, 5, 2, "POSITION : X: %.0f, Y: %.0f", host->x, host->y);

    // add visual seperation line
    mvwhline(telemetry_win, 7, 1, '-', max_x - 2);

    // intruder aircrafts telemetry data
    // calculate number of intruder aircrafts
    int active_count = 0;
    for (int i = 0; i < MAX_TRACK; ++i) {
        if (display_simWorld_buffer.intruders[i].isActive) {
            active_count++;
        }
    }

    char track_msg[50];
    sprintf(track_msg, "ACTIVE TRACKS: %d / %d", active_count, MAX_TRACK);

    wattron(telemetry_win, A_BOLD);
    mvwprintw(telemetry_win, 8, 2, "%s", track_msg);
    wattroff(telemetry_win, A_BOLD);

    int print_line = 10; // starting row for intruder telemetry data

    for (int i = 0; i < MAX_TRACK; ++i){
        const IntruderShip* intr = &display_simWorld_buffer.intruders[i];

        if (!intr->isActive){
            continue;
        } 

        // hide intruder aircraft entries that overflow from telemetry window
        // TODO: Handle telemetry pagination. Currently hides tracks that exceed window height.
        if (print_line >= max_y - 2){
            mvwprintw(telemetry_win, print_line, 2, "... (More tracks hidden)");
            break;
        } 

        double dx = intr->state.x - host->x;
        double dy = intr->state.y - host->y;
        double distance_m = sqrt((dx * dx) + (dy * dy)); // calculate slant distance
        double distance_nm = distance_m / 1852.0; // meter to nautical mile (nm) conversion

        // relative to host aircraft altitude 
        double rel_alt_m = intr->state.z - host->z;
        double rel_alt_ft = rel_alt_m * 3.28084; // meter to feet conversion

        // choose label and color by threat level
        char* threat_str = "PROX";
        int color_pair = 3; // default: proximate/other (cyan)

        switch (intr->metrics.threat_level){
            case THREAT_RA: // resolution advisory
                threat_str = " RA ";
                color_pair = 1; // red
                break;

            case THREAT_TA: // traffic advisory
                threat_str = " TA ";
                color_pair = 2; // yellow
                break;

            default:
                break; // keep defaults
        } // switch-case end

        // format: TRK ID | THREAT | DISTANCE | RELATIVE ALTITUDE (add mode_s in future)
        wattron(telemetry_win, COLOR_PAIR(color_pair));
        mvwprintw(telemetry_win, print_line, 2, "TRK %02d | %s | %4.1f NM | %+5.0f ft", i, threat_str, distance_nm, rel_alt_ft);
        wattroff(telemetry_win, COLOR_PAIR(color_pair));

        print_line += 2; // add 1 line interval between entries
    } // for end

    wnoutrefresh(telemetry_win); // queue window changes in memory
} // render_telemetry end

/**
 * @brief Render system status and control information.
 */
static void render_status(){
    // calculate render time and jitter
    static struct timespec last_time = {0};
    struct timespec current_time;

    clock_gettime(CLOCK_MONOTONIC, &current_time);

    if (last_time.tv_sec == 0){
        last_time = current_time;
    }

    // calculate elapsed time in milliseconds
    double elapsed_ms = (current_time.tv_sec - last_time.tv_sec) * 1000.0 + 
                        (current_time.tv_nsec - last_time.tv_nsec) / 1000000.0;
    
    // target ms : DISPLAY_REFRESH_RATE_US / 1000.0 (33.0 ms)
    double target_ms = DISPLAY_REFRESH_RATE_US / 1000.0;
    double jitter = elapsed_ms - target_ms; 

    // ncurses render operations
    // Bir sonraki frame için zamanı kaydet
    last_time = current_time;

    werase(status_win); // erase previous window frame

    int max_x = getmaxx(status_win); // fetch status window width (max_x)

    box(status_win, 0, 0); // draw window border

    // draw current TCAS mode
    // TODO: TCAS mode will be changeable in future
    wattron(status_win, COLOR_PAIR(4) | A_BOLD); // green and bold
    mvwprintw(status_win, 1, 2, "MODE: TA/RA");
    wattroff(status_win, COLOR_PAIR(4) | A_BOLD);

    // draw current range
    // TODO: TCAS range will be changeable in future
    wattron(status_win, COLOR_PAIR(3) | A_BOLD); // cyan and bold
    mvwprintw(status_win, 1, 14, "RNG: 30 NM");
    wattroff(status_win, COLOR_PAIR(3) | A_BOLD);

    // draw system status
    mvwprintw(status_win, 1, 25, "SYS: OK");

    mvwprintw(status_win, 1, max_x - 16, "CTRL+C to Exit");

    mvwprintw(status_win, 2, 2, "Frame Time: %5.2f ms | Jitter: %+6.2f ms", elapsed_ms, jitter);

    wnoutrefresh(status_win); // queue window changes in memory
} // render_status end

void* display_thread(void* arg){ // thread function
    init_display();

    struct timespec start_time, end_time; // used for dynamic sleep
    
    while (!isShutdownSignaled){
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        fetch_display_data();

        // render all windows in memory
        render_radar();
        render_telemetry();
        render_status();

        doupdate(); // output all rendered windows to terminal (to prevent flickering)

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        // calculate render duration for the frame in microseconds
        long work_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L + 
                            (end_time.tv_nsec - start_time.tv_nsec) / 1000L;

        // calculate remaining sleep time in microseconds
        long sleep_time_us = DISPLAY_REFRESH_RATE_US - work_time_us;

        // sleep if needed
        if (sleep_time_us > 0){
            usleep(sleep_time_us);
        }
    } // while end

    destroy_display();

    printf("[DISPLAY THREAD] Shutdown Successful\n");

    return NULL;
} // display_thread end