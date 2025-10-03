#pragma once

// TODO: add func/names for timing spots here
#define TIMING_SPOTS(X) \
    X(SendQuery_func)       \
    X(ExecQueryAndProcessResults_func) \
    X(ExecQueryAndProcessResults_first_result) \
    X(ExecQueryAndProcessResults_result_loop) \
    X(SomethingElseFunc) \
    X(PQsendQueryParams_func) \
    X(PQsendPrepare_func) \
    X(PQsendQueryPrepared_func) \
    X(getAnotherTuple_func) \
    X(CopyFrom_overall) \
    X(CopyFrom_putdata_loop) \
    X(CopyFrom_putdata_single) \
    X(CopyTo_overall) \
    X(CopyTo_getdata_loop) \
    X(CopyTo_getdata_single) \
    X(Connection_send_data) \
    X(Connection_recv_data) \
    X(Connection_parseInput) \
    X(Serializer_copy_data_framing) \
    X(Serializer_protocol_header) \
    X(Deserializer_message_parse) \
    X(Deserializer_data_extract) \
    X(BufferMgr_check_space) \
    X(BufferMgr_allocate) \
    X(BufferMgr_flush) \
    X(BufferMgr_memcpy) \
    X(Session_getCopyDataMessage) \
    X(Session_putCopyData_prep)

// --- Use the list to generate the enum ---
#define AS_ENUM(name) name,

typedef enum {
    TIMING_SPOTS(AS_ENUM)
    _NUM_TIMING_SPOTS // get the count of timing spots
} timing_spot_enums;

// --- Use the same list to generate the names array ---
#define AS_STRING(name) #name,
