#pragma once

// TODO: add func/names for timing spots here
#define TIMING_SPOTS(X) \
    X(SendQuery_func)       \
    X(ExecQueryAndProcessResults_func) \
    X(ExecQueryAndProcessResults_first_result) \
    X(ExecQueryAndProcessResults_result_loop) \
    X(SomethingElseFunc)

// --- Use the list to generate the enum ---
#define AS_ENUM(name) name,

typedef enum {
    TIMING_SPOTS(AS_ENUM)
    _NUM_TIMING_SPOTS // get the count of timing spots
} timing_spot_enums;

// --- Use the same list to generate the names array ---
#define AS_STRING(name) #name,
