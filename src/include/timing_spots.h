#pragma once

#include "./timing_custom_stats.h"

#define VERBOSE_TIMING_SPOTS 1 // set to 1 to get more detailed timing spots

/*
 * Helper macros to conditionally emit verbose timing calls. Use
 * verbose_timing_start(<spot>) and verbose_timing_end(<spot>).
 */
#ifndef verbose_timing_start
#if VERBOSE_TIMING_SPOTS
#define verbose_timing_start(spot) timing_start(spot)
#define verbose_timing_end(spot) timing_end(spot)
#else
/* no-op when verbose timing is disabled */
#define verbose_timing_start(spot) ((void)0)
#define verbose_timing_end(spot) ((void)0)
#endif
#endif

// TODO: add func/names for timing spots here
/* Notes:
 * pull pair: FetchIntermediate_ & SendViaCopy_ sending/receiving intermediate results
 *   - receiving tuples via copy: FetchIntermediate_CopyData_ & FetchIntermediate_File_ receiving and writing to file
 * push pair: DestReceiver_All & ExecutePlanIntoDestReceiver_ executing and pushing intermediate results
 *   - target receive via copy: ReceiveViaCopy_ & ReceiveAndWriteCopyData_ receiving and writing to file
 */
// DoCopyFromLocalTableIntoShards: the actual loop that copies data
// CitusSendTupleToPlacements_: serializing and sending out the tuple
// SendViaCopy_: calls FileReadCompat to read from file and just send
//
// RemoteFileDestReceiver_Init may include writing to local file, plus setting up connections
// ReceiveResults_HeapFormTuple happens during ReceiveResults_BuildTuples, and is a PG call, subtracting its time
// reflects the rest of deserialzation time
#define TIMING_SPOTS(X)                                                                                                \
    X(ExecSimpleQuery)                                                                                                 \
    X(ParseQuery)                                                                                                      \
    X(QueryAnalyzeAndRewrite)                                                                                          \
    X(QueryExecution)                                                                                                  \
    X(QueryPlanning)                                                                                                   \
    X(EndingComms)                                                                                                     \
    X(Printtup_Startup)                                                                                                \
    X(Printtup)                                                                                                        \
    X(Printtup_Net)                                                                                                    \
    X(PQ_putmessage)                                                                                                   \
    X(CreateCitusTable_)                                                                                               \
    X(CopyFromLocalTableIntoDistTable_)                                                                                \
    X(DoCopyFromLocalTableIntoShards_)                                                                                 \
    X(CitusSendTupleToPlacements_)                                                                                     \
    X(SerializeAndCopyRow_)                                                                                            \
    X(SendCopyDataToPlacement_)                                                                                        \
    X(WriteTupleToLocal_)                                                                                              \
    X(DoLocalCopy_)                                                                                                    \
    X(Receiver_CopyFrom)                                                                                               \
    X(NextCopyFrom_)                                                                                                   \
    X(CopyFromInsertIntoTable)                                                                                         \
                                                                                                                       \
    X(FetchIntermediate_)                                                                                              \
    X(FetchIntermediate_CopyAndWrite)                                                                                  \
    X(FetchIntermediate_FileWrite)                                                                                     \
                                                                                                                       \
    X(ReceiveAndWriteCopyData_)                                                                                        \
    X(ReceiveAndWriteCopyData_Deser)                                                                                   \
    X(ReceiveAndWriteCopyData_WriteFile)                                                                               \
    X(SendViaCopy_)                                                                                                    \
    X(SendViaCopy_Send)                                                                                                \
    X(SendViaCopy_FileRead)                                                                                            \
                                                                                                                       \
    X(ExecutePlanIntoColocatedIntermediateResults_)                                                                    \
    X(ExecutePlanIntoDestReceiver_)                                                                                    \
                                                                                                                       \
    X(ReceiveResults_)                                                                                                 \
    X(ReceiveResults_Net)                                                                                              \
    X(ReceiveResults_Deserialize)                                                                                      \
    X(ReceiveResults_BuildTuples)                                                                                      \
    X(ReceiveResults_HeapFormTuple)                                                                                    \
                                                                                                                       \
    X(RemoteFileDestReceiver_Init)                                                                                     \
    X(RemoteFileDestReceiver_SerAndSend)                                                                               \
    X(RemoteFileDestReceiver_Ser)                                                                                      \
    X(RemoteFileDestReceiver_Send)                                                                                     \
    X(RemoteFileDestReceiver_SerAndSend_WriteLocal)                                                                    \
                                                                                                                       \
    X(ProcessCopyStmt_)                                                                                                \
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
