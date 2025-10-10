#include "postgres_fe.h"

#include "libpq/dbcomm_time_instr.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h> // For gettimeofday in log_message

#include "libpq/dbcomm_timing_spots.h"

static const char *const timing_spot_names[_NUM_TIMING_SPOTS] = {
    TIMING_SPOTS(AS_STRING)
};

const char *const *
logger_get_timing_spot_names(void)
{
    return timing_spot_names;
}

// Internal structure to hold data for a single timer
typedef struct {
    const char* name;
    uint64_t total_ns;
    uint64_t count;
    struct timespec start_time;
} timer_stat_t;

// Global state for the logger
static struct {
    int num_timers;
    timer_stat_t* stats;
} logger_state = {0, NULL};


int logger_init(int num_timers, const char* names[]) {
    if (logger_state.stats != NULL) {
        // Already initialized
        return 0;
    }
    
    logger_state.num_timers = num_timers;
    logger_state.stats = (timer_stat_t*)calloc(num_timers, sizeof(timer_stat_t));
    
    if (logger_state.stats == NULL) {
        perror("Failed to allocate memory for logger stats");
        return -1;
    }
    
    for (int i = 0; i < num_timers; ++i) {
        logger_state.stats[i].name = names[i] ? names[i] : "Unnamed Timer";
    }
    
    return 0;
}

void timing_start(int timer_id) {
    if (timer_id < 0 || timer_id >= logger_state.num_timers) return;
    clock_gettime(CLOCK_MONOTONIC_RAW, &logger_state.stats[timer_id].start_time);
}

void timing_end(int timer_id) {
    if (timer_id < 0 || timer_id >= logger_state.num_timers) return;

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);

    uint64_t start_ns = (uint64_t)logger_state.stats[timer_id].start_time.tv_sec * 1000000000 +
                        logger_state.stats[timer_id].start_time.tv_nsec;
    uint64_t end_ns = (uint64_t)end_time.tv_sec * 1000000000 + end_time.tv_nsec;
    
    uint64_t elapsed_ns = end_ns - start_ns;

    logger_state.stats[timer_id].total_ns += elapsed_ns;
    logger_state.stats[timer_id].count++;
}

void logger_print_timings(void) {
    if (logger_state.stats == NULL) {
        printf("Logger not initialized\n");
        return;
    }

    // Calculate communication stack timings before printing
    // COPY FROM communication stack = sum of individual comm components
    uint64_t copyfrom_comm_total = 
        logger_state.stats[Connection_parseInput].total_ns +
        logger_state.stats[Connection_send_data].total_ns +
        logger_state.stats[BufferMgr_check_space].total_ns +
        logger_state.stats[BufferMgr_flush].total_ns +
        logger_state.stats[BufferMgr_realloc_output].total_ns +
        logger_state.stats[BufferMgr_memcpy].total_ns +
        logger_state.stats[Serializer_protocol_header].total_ns +
        logger_state.stats[Serializer_finalize_length].total_ns;
    
    uint64_t copyfrom_comm_count = 1; // Single measurement for the entire operation
    
    // COPY TO communication stack = sum of individual comm components  
    uint64_t copyto_comm_total =
        logger_state.stats[Session_getCopyDataMessage].total_ns +
        logger_state.stats[Connection_recv_data].total_ns +
        logger_state.stats[Deserializer_message_parse].total_ns +
        logger_state.stats[Deserializer_data_extract].total_ns +
        logger_state.stats[BufferMgr_allocate_input].total_ns;
        
    uint64_t copyto_comm_count = 1; // Single measurement for the entire operation
    
    // Store the calculated values in the stats array
    logger_state.stats[CopyFrom_comm_stack].total_ns = copyfrom_comm_total;
    logger_state.stats[CopyFrom_comm_stack].count = copyfrom_comm_count;
    logger_state.stats[CopyFrom_comm_stack].name = "CopyFrom_comm_stack";
    
    logger_state.stats[CopyTo_comm_stack].total_ns = copyto_comm_total;
    logger_state.stats[CopyTo_comm_stack].count = copyto_comm_count;
    logger_state.stats[CopyTo_comm_stack].name = "CopyTo_comm_stack";

    // Print header for nanosecond timing report
    printf("\n--- Timing Report (Nanoseconds) ---\n");
    printf("%-30s | %10s | %18s | %18s\n", "Timer Name", "Count", "Total Time (ns)", "Average Time (ns)");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < logger_state.num_timers; ++i) {
        timer_stat_t* stat = &logger_state.stats[i];
        if (stat->count == 0) continue;

        // Use nanoseconds for both total and average
        uint64_t total_ns = stat->total_ns;
        uint64_t avg_ns = stat->count ? (stat->total_ns / stat->count) : 0;

        printf("%-30s | %10llu | %18llu | %18llu\n",
               stat->name,
               (unsigned long long)stat->count,
               (unsigned long long)total_ns,
               (unsigned long long)avg_ns);
    }
    printf("----------------------------------------------------------------------------------------\n");
    
    // free(logger_state.stats);
    // logger_state.stats = NULL;
    // logger_state.num_timers = 0;
}


void log_message_internal(const char* file, int line, const char* format, ...) {
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
