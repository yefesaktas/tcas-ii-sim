/**
 * @file main.c
 * @brief 
 *
 * 
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 */

#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "storage.h"
#include "transponder_data.h"
// #include "tcas_logic.h"

// SIGINT and SIGTERM signal handler
void SIGINT_SIGTERM_handlerRoutine(int sig){
    int saved_errno = errno; // save current system error context

    isShutdownSignaled = 1; // async-signal-safe

    errno = saved_errno; // reload current system error context
} // SIGINT_SIGTERM_handlerRoutine end

int main(void){
    // sigaction struct 
    struct sigaction sa;
    sa.sa_handler = SIGINT_SIGTERM_handlerRoutine;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask); // do not mask other signals while running the handler routine

    if (sigaction(SIGINT, &sa, NULL) == -1){
        perror("-- [MAIN THREAD] sigaction SIGINT");
        return EXIT_FAILURE;
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1){
        perror("-- [MAIN THREAD] sigaction SIGTERM");
        return EXIT_FAILURE;
    }

    // initalize storage primitives
    init_buffer();

    // define threads
    pthread_t disp_tid;
    pthread_t transponder_tid;
    // pthread_t tcas_logic_tid;

    // start threads
    printf("[MAIN THREAD] Initializing system threads..\n");

    if (pthread_create(&disp_tid, NULL, display_thread, NULL) != 0) {
        perror("[MAIN THREAD] Failed to create display thread");
        return EXIT_FAILURE;
    }

    if (pthread_create(&transponder_tid, NULL, transponder_data_thread, NULL) != 0) {
        perror("[MAIN THREAD] Failed to create transponder thread");
        return EXIT_FAILURE;
    }

    /*
    if (pthread_create(&tcas_logic_tid, NULL, , NULL) != 0) {
        perror("[MAIN THREAD] Failed to create tcas logic thread");
        return EXIT_FAILURE;
    }
    */

    // wait until a shutdown is requested
    while (!isShutdownSignaled){
        usleep(100000); // 100 ms 
    }

    // initiate graceful shutdown procedure
    pthread_join(disp_tid, NULL);

    printf("\n[MAIN THREAD] Shutdown signal received. Joining threads...\n");
    fflush(stdout);
    
    pthread_join(transponder_tid, NULL);
    // pthread_join(tcas_logic_tid, NULL);
    
    finalize_buffer(); // finalize storage primitives

    printf("[MAIN THREAD] System shutdown gracefully.\n");

    return EXIT_SUCCESS;
} // main end