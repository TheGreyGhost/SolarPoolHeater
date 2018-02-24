#ifndef DATALOG_H   
#define DATALOG_H  
#include <Arduino.h>

enum LogfileStatus {LFS_CARD_NOT_PRESENT = 0, LFS_FAILED_TO_OPEN = 1, LFS_WRITE_FAILED = 2, LFS_OK = 3};
extern const char* logfileStatusText[4];

extern LogfileStatus logfileStatus;

void setupDatalog();
void tickDatalog();

bool dataLogErase();
unsigned long dataLogNumberOfSamples();
void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries);
#endif

