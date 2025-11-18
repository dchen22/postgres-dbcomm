/**
 * Custom statistics keys for timing instrumentation.
 *
 * This file defines the enum keys for custom statistics that can be tracked
 * alongside timing measurements. Add your custom stat keys here using the
 * CUSTOM_STATS macro.
 *
 * Example:
 *   X(STAT_TOTAL_BYTES_PROCESSED, "total_bytes_processed") \
 *   X(STAT_TOTAL_BYTES_SENT, "total_bytes_sent") \
 */

#pragma once
#ifndef CUSTOM_STATS_H
#define CUSTOM_STATS_H

/* Define custom statistics here using X-macro pattern
 * Format: X(ENUM_NAME, "display_name")
 */
#define CUSTOM_STATS(X)                                                                                                \
    X(STAT_TOTAL_BYTES_SERIALIZED, "total_bytes_serialized")                                                           \
    X(STAT_TOTAL_BYTES_DESERIALIZED, "total_bytes_deserialized")                                                       \
    X(STAT_TOTAL_BYTES_SENT, "total_bytes_sent")                                                                       \
    X(STAT_TOTAL_BYTES_RECEIVED, "total_bytes_received")                                                               \
    X(STAT_TOTAL_ROWS_SERIALIZED, "total_rows_serialized")                                                             \
    X(STAT_TOTAL_ROWS_DESERIALIZED, "total_rows_deserialized")                                                         \
    X(STAT_TOTAL_ROWS_SENT, "total_rows_sent")                                                                         \
    X(STAT_TOTAL_ROWS_RECEIVED, "total_rows_received")                                                                 \
    X(STAT_OTHERS, "add others here")                                                                                  \
    /* Add more custom stats above this line */

/* Generate enum values */
#define STATS_AS_ENUM(name, str) name,
typedef enum
{
    CUSTOM_STATS(STATS_AS_ENUM) _NUM_CUSTOM_STATS
} custom_stat_key_t;
// #undef STATS_AS_ENUM

/* Generate string names array (defined in time_instr.c) */
#define STATS_AS_STRING(name, str) str,
extern const char *custom_stat_names[_NUM_CUSTOM_STATS];
// #undef STATS_AS_STRING

#endif // CUSTOM_STATS_H
