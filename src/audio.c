/**
 * @file audio.c
 * @brief 
 *
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include "audio.h"
#include <stdlib.h>
#include <stdio.h>

// stores the previous threat level, initialized as THREAT_NONE
static ThreatLevel previous_level = THREAT_NONE;

void process_audio_alerts(ThreatLevel current_level){
    // if the previous threat level remains, do not play any audio to prevent audio spams
    if (current_level == previous_level){
        return;
    }

    int ret; // dummy variable for the return value of system()

    // play the corresponding audio wrt the new threat level (uses aplay)
    switch (current_level){
        case THREAT_TA:
            // "Traffic! Traffic!"
            ret = system("aplay assets/sounds/traffic_x3.wav > /dev/null 2>&1 &");
            (void)ret;

            break;

        case THREAT_RA:
            // "Climb! Climb!"
            ret = system("aplay assets/sounds/climb_now_x2.wav > /dev/null 2>&1 &");
            (void)ret;

            break;

        case THREAT_NONE:
            // "Clear of Conflict"
            ret = system("aplay assets/sounds/clear_of_conflict.wav > /dev/null 2>&1 &");
            (void)ret;

            break;
    }

    previous_level = current_level;
} // process_audio_alerts end