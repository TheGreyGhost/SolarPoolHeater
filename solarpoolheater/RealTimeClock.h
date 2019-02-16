#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H
#include <Arduino.h>
#include <RTClib.h>

void setupRTC();
void tickRTC();

enum RESYNCHstatus {RESYNCH_not_required = 0, RESYNCH_synchronising = 1, RESYNCH_mismatch_too_big = 2, RESYNCH_error = 3};
const int NUMBER_OF_RESYNCH_STATES = 4;

// print the given date + time to the destination
void printDateTime(Print &dest, DateTime dateTime);

// prints the given date followed by the given timezone
void printDateTimeAndZone(Print &dest, DateTime dateTime, long timeZone);

// converts the given dateTimeUTC to the given timeZone, prints both
void printDateTimeWithZoneConversion(Print &dest, DateTime dateTimeUTC, long timeZone);

// Parses a date & time with timezone
// "Dec 26 2009 12:34:56 UTC+09:30" - must match this form and spacing and capitalisation exactly
// returns true for OK, false for a problem with the format
// retDateTime is the clock time in UTC+0:00, retTimeZone is the timezone in seconds eg +9:30 is 9.5 * 3600
bool parseDateTimeWithZone(const char newDateTimeWithZone[], DateTime &retDateTime, long &retTimeZone);

// set the current date + time to the given string
// MUST follow the following format (exact character count and spacing)
// "Dec 26 2009 12:34:56 UTC+09:30"
// returns false if a problem occurred (however most errors / format problems will fail silently and result in an unexpected date/time)
bool setDateTime(const char newDateTime[]);

// set the current date + time 
// newTimeUTC is the clock time in UTC+0:00, newTimezoneSeconds is the timezone in seconds eg +9:30 is 9.5 * 3600
bool setDateTime(DateTime newTimeUTC, long newTimezoneSeconds);

// set the correct time, subsequent calls to tickResynchronise will gradually adjust the clock
//  to match this resynchronisation time
// returns the current clock error relative to the synch time (+ve --> arduino is ahead of synch time)
long setDateTimeForResynchronisation(unsigned long unixtimeseconds, long timezoneseconds);

// resynchronise the clock (if required)
// resynchronisation is performed one second at a time
// this function should be called periodically (eg once per hour) to add or drop seconds
RESYNCHstatus tickResynchronise();

// what is the current resynchronisation status?
RESYNCHstatus getResynchStatus();

// returns a text description of the current resynchronisation state
const char *getResynchStatusLabel();

// returns the number of seconds that the RTC is ahead of the true time
long getSynchronisationMismatch();

// refresh currentTimeUTC, currentTimeWithZone
// returns true for success
bool refreshCurrentTime();

extern DateTime currentTimeUTC; // the current time in UTC+0:00
extern DateTime currentTimeWithZone; // the current time in the current timezone
extern long currentTimeZoneSeconds; // the seconds of the current timezone i.e. +9:30 = 9.5 * 3600
extern bool realTimeClockStatus;  // true if ok

const int DATETIMEFORMAT_DATELENGTH = 11; // "Dec 04 2018"
const int DATETIMEFORMAT_TIMELENGTH = 8; // "12:34:56"
const int DATETIMEFORMAT_TIMEZONE_LENGTH = 9; // "UTC+09:30"
const int DATETIMEFORMAT_TIME_STARTPOS = DATETIMEFORMAT_DATELENGTH + 1; // includes space
const int DATETIMEFORMAT_TIMEZONE_STARTPOS = DATETIMEFORMAT_TIME_STARTPOS + DATETIMEFORMAT_TIMELENGTH + 1; // one space
const int DATETIMEFORMAT_TOTALLENGTH = DATETIMEFORMAT_TIMEZONE_STARTPOS + DATETIMEFORMAT_TIMEZONE_LENGTH;

#endif
