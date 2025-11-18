#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h> // For gettimeofday in log_message

#include "time_instr.h"

#include "postgres.h"

#include "port.h" // for printf from PG

const char *timing_spot_names[_NUM_TIMING_SPOTS] = {TIMING_SPOTS(AS_STRING)};
const char *custom_stat_names[_NUM_CUSTOM_STATS] = {CUSTOM_STATS(STATS_AS_STRING)};

// Internal structure to hold data for a single timer
typedef struct
{
    const char *name;
    uint64_t total_ns;
    uint64_t count;
    struct timespec start_time;
    uint64_t custom_stats[_NUM_CUSTOM_STATS]; // Array of custom statistics indexed by stat_key
} timer_stat_t;

// Global state for the logger
static struct
{
    int num_timers;
    timer_stat_t *stats;
} logger_state = {0, NULL};

int logger_init(int num_timers, const char *names[])
{
    if (logger_state.stats != NULL)
    {
        // Already initialized
        return 0;
    }

    logger_state.num_timers = num_timers;
    logger_state.stats = (timer_stat_t *)calloc(num_timers, sizeof(timer_stat_t));

    if (logger_state.stats == NULL)
    {
        perror("Failed to allocate memory for logger stats");
        return -1;
    }

    for (int i = 0; i < num_timers; ++i)
    {
        int j;
        logger_state.stats[i].name = names[i] ? names[i] : "Unnamed Timer";
        /* Initialize all custom stats to 0 */
        for (j = 0; j < _NUM_CUSTOM_STATS; ++j)
        {
            logger_state.stats[i].custom_stats[j] = 0;
        }
    }

    return 0;
}

/**
 * @brief Adds a value to a custom statistic for a specific timer.
 *
 * This function allows tracking additional metrics beyond just timing,
 * such as bytes processed, bytes sent, etc. The stat is identified by
 * an enum key (defined in custom_stats.h), and the value is accumulated.
 *
 * @param timer_id The timer ID to add the stat to.
 * @param stat_key The custom_stat_key_t enum value for the stat.
 * @param value The value to add to the stat.
 */
void timing_add_stat(int timer_id, int stat_key, uint64_t value)
{
    if (timer_id < 0 || timer_id >= logger_state.num_timers || logger_state.stats == NULL)
        return;

    if (stat_key < 0 || stat_key >= _NUM_CUSTOM_STATS)
        return;

    logger_state.stats[timer_id].custom_stats[stat_key] += value;
}
void timing_start(int timer_id)
{
    if (timer_id < 0 || timer_id >= logger_state.num_timers)
        return;
    clock_gettime(CLOCK_MONOTONIC_RAW, &logger_state.stats[timer_id].start_time);
}

void timing_end(int timer_id)
{
    if (timer_id < 0 || timer_id >= logger_state.num_timers)
        return;

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);

    uint64_t start_ns = (uint64_t)logger_state.stats[timer_id].start_time.tv_sec * 1000000000 +
                        logger_state.stats[timer_id].start_time.tv_nsec;
    uint64_t end_ns = (uint64_t)end_time.tv_sec * 1000000000 + end_time.tv_nsec;

    uint64_t elapsed_ns = end_ns - start_ns;

    logger_state.stats[timer_id].total_ns += elapsed_ns;
    logger_state.stats[timer_id].count++;
}

void logger_print_timings(void)
{
    if (logger_state.stats == NULL)
    {
        printf("Logger not initialized\n");
        return;
    }

    // Print header for nanosecond timing report
    printf("\n--- Timing Report (Nanoseconds) ---\n");
    printf("%-30s | %10s | %18s | %18s | %s\n", "Timer Name", "Count", "Total Time (ns)", "Average Time (ns)",
           "Custom Stats");
    printf("-----------------------------------------------------------------------------------------------------------"
           "---\n");

    for (int i = 0; i < logger_state.num_timers; ++i)
    {
        timer_stat_t *stat = &logger_state.stats[i];
        if (stat->count == 0)
            continue;

        // Use nanoseconds for both total and average
        uint64_t total_ns = stat->total_ns;
        uint64_t avg_ns = stat->count ? (stat->total_ns / stat->count) : 0;

        printf("%-30s | %10llu | %18llu | %18llu | ", stat->name, (unsigned long long)stat->count,
               (unsigned long long)total_ns, (unsigned long long)avg_ns);

        /* Print custom stats in comma-separated key=value format */
        int first = 1;
        int j;
        for (j = 0; j < _NUM_CUSTOM_STATS; ++j)
        {
            /* Only print non-zero stats */
            if (stat->custom_stats[j] > 0)
            {
                if (!first)
                {
                    printf(", ");
                }
                printf("%s=%llu", custom_stat_names[j], (unsigned long long)stat->custom_stats[j]);
                first = 0;
            }
        }
        printf("\n");
    }
    printf("-----------------------------------------------------------------------------------------------------------"
           "---\n");

    free(logger_state.stats);
    logger_state.stats = NULL;
    logger_state.num_timers = 0;
}

void logger_cleanup()
{
    if (logger_state.stats != NULL)
    {
        free(logger_state.stats);
        logger_state.stats = NULL;
        logger_state.num_timers = 0;
    }
}

void log_message_internal(const char *file, int line, const char *format, ...)
{
    // Get current time for the log message timestamp
    char time_buf[32];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    strftime(time_buf, sizeof(time_buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

    // Add milliseconds
    int len = strlen(time_buf);
    snprintf(time_buf + len, sizeof(time_buf) - len, ".%03ld", tv.tv_usec / 1000);

    // Print the file, line, timestamp, and the user's message
    printf("[%s:%d] [%s] ", file, line, time_buf);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
