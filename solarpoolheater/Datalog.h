#ifndef DATALOG_H   
#define DATALOG_H
#include "GlobalDefines.h"  
#include <Arduino.h>

enum LogfileStatus {LFS_CARD_NOT_PRESENT = 0, LFS_FAILED_TO_OPEN = 1, LFS_WRITE_FAILED = 2, LFS_OK = 3};
extern const char* logfileStatusText[4];

extern LogfileStatus logfileStatus;

void setupDatalog();
void tickDatalog();

bool dataLogErase();
unsigned long dataLogNumberOfSamples();

// print the given entries to dest, in the format 
// probe1 min max avg {probeSeparator} probe2 min max avg etc
void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries, const char probeSeparator[]);
#endif
