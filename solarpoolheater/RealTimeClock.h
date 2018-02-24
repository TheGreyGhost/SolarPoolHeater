#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H
#include <Arduino.h>
#include <RTClib.h>

void setupRTC();
void tickRTC();

// print the given date + time to the destination
void printDateTime(Print &dest, DateTime dateTime);

// set the current date + time to the given string
// MUST follow the following format (exact character count and spacing)
// "Dec 26 2009 12:34:56"
void setDateTime(const char newDateTime[]);

extern DateTime currentTime; // the current time
extern bool realTimeClockStatus;  // true if ok

#endif
