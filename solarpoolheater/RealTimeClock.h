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
// "Dec 26 2009 12:34:56 UTC+09:30"
void setDateTime(const char newDateTime[]);

// set the correct time, subsequent calls to tickResynchronise will gradually adjust the clock
//  to match this resynchronisation time
// returns the current clock error relative to the synch time (+ve --> arduino is ahead of synch time)
unsigned long setDateTimeForResynchronisation(unsigned long unixtimeseconds, long timezoneseconds);

// resynchronise the clock (if required)
// resynchronisation is performed one second at a time
// this function should be called periodically (eg once per hour) to add or drop seconds
void tickResynchronise();

extern DateTime currentTime; // the current time
extern bool realTimeClockStatus;  // true if ok

const int DATETIMEFORMAT_DATELENGTH = 11; // "Dec 04 2018"
const int DATETIMEFORMAT_TIMELENGTH = 8; // "12:34:56"
const int DATETIMEFORMAT_TIMEZONE_LENGTH = 9; // "UTC+09:30"
const int DATETIMEFORMAT_TIME_STARTPOS = DATETIMEFORMAT_DATELENGTH + 1; // includes space
const int DATETIMEFORMAT_TIMEZONE_STARTPOS = DATETIMEFORMAT_TIME_STARTPOS + DATETIMEFORMAT_TIMELENGTH + 1; // includes space
const int DATETIMEFORMAT_TOTALLENGTH = DATETIMEFORMAT_TIMEZONE_STARTPOS + DATETIMEFORMAT_TIMEZONE_LENGTH;
#endif
