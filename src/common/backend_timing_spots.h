#pragma once

// TODO: add func/names for timing spots here
// HandshakeComms: sending tuple desc first, and EndCommand etc.
#define TIMING_SPOTS(X)                                                                                                \
    X(ExecSimpleQuery)                                                                                                 \
    X(ParseQuery)                                                                                                      \
    X(QueryAnalyzeAndRewrite)                                                                                          \
    X(QueryExecution)                                                                                                  \
    X(QueryPlanning)                                                                                                   \
    X(EndingComms)                                                                                                     \
    X(Printtup_Startup)                                                                                                \
    X(Printtup)                                                                                                        \
    X(PQ_putmessage)                                                                                                     \
    X(SomethingElseFunc)

// --- Use the list to generate the enum ---
#define AS_ENUM(name) name,

typedef enum
{
    TIMING_SPOTS(AS_ENUM) _NUM_TIMING_SPOTS // get the count of timing spots
} timing_spot_enums;

// --- Use the same list to generate the names array ---
#define AS_STRING(name) #name,

extern const char *timing_spot_names[_NUM_TIMING_SPOTS];
