#pragma once

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
#define TIMING_SPOTS(X)                                                                                                \
    X(ExecSimpleQuery)                                                                                                 \
    X(ParseQuery)                                                                                                      \
    X(QueryAnalyzeAndRewrite)                                                                                          \
    X(QueryExecution)                                                                                                  \
    X(QueryPlanning)                                                                                                   \
    X(EndingComms)                                                                                                     \
    X(Printtup_Startup)                                                                                                \
    X(Printtup)                                                                                                        \
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
    X(FetchIntermediate_CopyData_)                                                                                     \
    X(FetchIntermediate_File_)                                                                                         \
    X(ReceiveViaCopy_)                                                                                                 \
    X(ReceiveAndWriteCopyData_)                                                                                        \
    X(SendViaCopy_)                                                                                                    \
    X(FileReadCompat_)                                                                                                 \
                                                                                                                       \
    X(ExecutePlanIntoColocatedIntermediateResults_)                                                                    \
    X(ExecutePlanIntoDestReceiver_)                                                                                    \
                                                                                                                       \
    X(ReceiveResults_)                                                                                                 \
    X(ReceiveResults_Net)                                                                                              \
    X(ReceiveResults_Deserialize)                                                                                      \
    X(ReceiveResults_BuildTuples)                                                                                      \
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
