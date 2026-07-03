/**
 * @file tcas_logic.h
 * @brief TCAS advisory computation thread interface.
 *
 * Declares the worker thread that reads the shared simulation state,
 * computes intruder metrics, and publishes TCAS advisory data.
 * @author Yusuf Efe Aktaş
 * @date 2026-01-27
 *
 * \msc
 * TCASLogicThread, Storage;
 * TCASLogicThread->Storage [label="get_buffer_snapshot() / update_intruders_tcas_data()"];
 * \endmsc
 */

#ifndef TCAS_LOGIC_H
#define TCAS_LOGIC_H

/**
 * @brief TCAS advisory thread entry point.
 *
 * @param arg Optional thread argument (unused, may be NULL).
 * @return void* Always returns NULL when the thread exits normally.
 *
 * \msc
 * TCASLogicThread, Storage;
 * TCASLogicThread->Storage [label="get_buffer_snapshot()"];
 * TCASLogicThread->Storage [label="update_intruders_tcas_data()"];
 * \endmsc
 */
void* tcas_logic_thread(void* arg);

#endif