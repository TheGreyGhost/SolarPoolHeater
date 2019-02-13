#include "RealTimeClock.h"
#include <Wire.h>

RTC_DS1307 realTimeClock;
DateTime currentTime;
bool realTimeClockStatus;

void setupRTC(void){
  Wire.begin();
  realTimeClock.begin();
  currentTime = realTimeClock.now();
}

void tickRTC() {
  realTimeClockStatus = realTimeClock.isrunning();
  if (realTimeClockStatus) {
    currentTime = realTimeClock.now();
  }
}

void printDateTime(Print &dest, DateTime dateTime) {
  dest.print(dateTime.year(), DEC);
  dest.print('/');
  dest.print(dateTime.month(), DEC);
  dest.print('/');
  dest.print(dateTime.day(), DEC);
  dest.print(' ');
  dest.print(dateTime.hour(), DEC);
  dest.print(':');
  dest.print(dateTime.minute(), DEC);
  dest.print(':');
  dest.print(dateTime.second(), DEC);
  dest.println();
}

byte gettwodigit(const char* p) {
    byte msd = 0;
    byte lsd = 0;
    if ('0' <= *p && *p <= '9')
        msd = *p - '0';
    ++p;    
    if ('0' <= *p && *p <= '9')
        lsd = *p - '0';
        
    return 10 * msd + lsd;
}

// "Dec 26 2009 12:34:56UTC+09:30"
// returns true for OK, false for a problem with the format
bool setDateTime(const char newDateTime[])
{
  if (strlen(newDateTime) < DATETIMEFORMAT_TOTALLENGTH) return false;
  
  DateTime newRawTime(newDateTime, newDateTime + DATETIMEFORMAT_TIME_STARTPOS);

  unsigned int newTimeZone = 3600 * gettwodigit(newDateTime + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4)
                             + 60 * gettwodigit(newDateTime + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4 + 2 + 1);
  if (newDateTime[DATETIMEFORMAT_TIMEZONE_STARTPOS + 3] == '-') {
    newTimeZone *= -1;
  }
  
  DateTime newTime = newRawTime - TimeSpan(newTimeZone);
  realTimeClock.adjust(newTime);
  
  return true;
}

const int DATETIMEFORMAT_DATELENGTH = 11; // "Dec 04 2018"
const int DATETIMEFORMAT_TIMELENGTH = 8; // "12:34:56"
const int DATETIMEFORMAT_TIMEZONE_LENGTH = 9; // "UTC+09:30"
const int DATETIMEFORMAT_TIME_STARTPOS = DATETIMEFORMAT_DATELENGTH + 1; // includes space
const int DATETIMEFORMAT_TIMEZONE_STARTPOS = DATETIMEFORMAT_TIME_STARTPOS + DATETIMEFORMAT_TIMELENGTH + 1; // includes space
const int DATETIMEFORMAT_TOTALLENGTH = DATETIMEFORMAT_TIMEZONE_STARTPOS + DATETIMEFORMAT_TIMEZONE_LENGTH;

  
}

// set the correct time, subsequent calls to tickResynchronise will gradually adjust the clock
//  to match this resynchronisation time
// returns the current clock error relative to the synch time (+ve --> arduino is ahead of synch time)
unsigned long setDateTimeForResynchronisation(unsigned long unixtimeseconds, long timezoneseconds);

// resynchronise the clock (if required)
// resynchronisation is performed one second at a time
// this function should be called periodically (eg once per hour) to add or drop seconds
void tickResynchronise();
