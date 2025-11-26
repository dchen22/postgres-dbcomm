/**
 * --- USAGE ---
 * The API for timing remains the same. The log_message function is now a macro,
 * but is called in the exact same way.
 *
 * // In any file:
 * #include "dbcomm_time_instr.h"
 * remember to call logger_init() once at the beginning somewhere
 * Then, to time a code section: timing_start(TIMER_ID); ...
 * timing_end(TIMER_ID); Finally, to print the report: logger_print_timings();
 *
 * // note: logging a single message like so
 * log_message("Starting process with value %d...", 42);
 * // Output will now include file and line number.
 */
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "./timing_spots.h"

// --- API ---

/**
 * @brief Initializes the logging and timing system.
 *
 * This function must be called once before any other logger or timing function.
 * It allocates the necessary internal structures.
 *
 * @param num_timers The total number of timers, typically from the user-defined enum (_NUM_TIMERS).
 * @param names An array of strings containing the names for each timer ID.
 * @return 0 on success, -1 on failure (e.g., memory allocation failed).
 */
int logger_init(int num_timers, const char *names[]);

/**
 * @brief Records the start time for a specific timer ID.
 *
 * This function is designed to be very fast and does not perform any memory allocations.
 * It uses clock_gettime(CLOCK_MONOTONIC_RAW).
 *
 * @param timer_id The enum value of the timer to start.
 */
void timing_start(int timer_id);

/**
 * @brief Records the end time for a specific timer ID and accumulates the result.
 *
 * This function calculates the elapsed time since the corresponding timing_start() call
 * and adds it to the total for that timer ID, also incrementing the call count.
 *
 * @param timer_id The enum value of the timer to end.
 */
void timing_end(int timer_id);

/**
 * @brief Adds a value to a custom statistic for a specific timer.
 *
 * This function allows tracking additional metrics beyond just timing,
 * such as bytes processed, bytes sent, etc. The stat is identified by
 * an enum key (defined in custom_stats.h), and the value is accumulated.
 *
 * Example usage:
 *   timing_add_stat(TIMER_NETWORK, STAT_TOTAL_BYTES_PROCESSED, 1024);
 *   timing_add_stat(TIMER_NETWORK, STAT_TOTAL_BYTES_SENT, 512);
 *
 * @param timer_id The enum value of the timer to add the stat to.
 * @param stat_key The custom_stat_key_t enum value for the stat.
 * @param value The value to add to the stat (accumulated across calls).
 */
void timing_add_stat(int timer_id, int stat_key, uint64_t value);

/**
 * @brief Prints a formatted report of all timing measurements and cleans up resources.
 *
 * Displays the name, call count, total time, and average time for each timer.
 * This function also frees the memory allocated by logger_init().
 */
void logger_print_timings(void);

void logger_cleanup();

void logger_reset();

size_t LoggerShmemSize(void);
void LoggerShmemInit(void);

/**
 * @brief The internal implementation of the logger. Do not call this directly.
 * Use the log_message() macro instead.
 */
void log_message_internal(const char *file, int line, const char *format, ...);

/**
 * @brief Logs a general-purpose message, similar to printf.
 *
 * This is a macro that automatically captures the file name and line number.
 * It prepends the file, line, and a wall-clock timestamp to the message.
 *
 * @param format The format string.
 * @param ... Variadic arguments for the format string.
 */
#define log_message(format, ...) log_message_internal(__FILE__, __LINE__, format, ##__VA_ARGS__)

#endif // LOGGER_H
