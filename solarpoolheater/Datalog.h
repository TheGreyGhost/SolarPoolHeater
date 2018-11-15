#ifndef DATALOG_H   
#define DATALOG_H
#include <Arduino.h>

enum LogfileStatus {LFS_OK = 0, LFS_CARD_NOT_PRESENT = 1, LFS_FAILED_TO_OPEN = 2, LFS_WRITE_FAILED = 3};
extern const char* logfileStatusText[4];

extern LogfileStatus logfileStatus;

void setupDatalog();
void tickDatalog();

bool dataLogErase();
unsigned long dataLogNumberOfSamples();

// print the given entries to dest, in the format 
// probe1 min max avg {probeSeparator} probe2 min max avg etc
void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries, const char probeSeparator[]);

// print the given entry to dest, in the format of raw bytes from the log file
// returns 0 for success or other for failure code:
// 1 = LFS_CARD_NOT_PRESENT, 2 = LFS_FAILED_TO_OPEN, 3 = LFS_WRITE_FAILED, 4 = invalid logfile number, 5 = read failed, 6 = too few bytes read, 7= write failed, 8 = seek failed
int dataLogPrintEntryBytes(Print &dest, long startidx);

#endif
