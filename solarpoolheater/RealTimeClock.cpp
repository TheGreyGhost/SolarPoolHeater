#include "RealTimeClock.h"
#include <Wire.h>
#include "Settings.h"
RTC_DS1307 realTimeClock;
DateTime currentTimeUTC;
DateTime currentTimeWithZone;
long currentTimeZoneSeconds;  // eg UTC+9:30 is 9.5 * 3600.  Also saved in EEPROM.

long timeMismatch;  // seconds that the RTC is ahead of the true time
unsigned long timeOfLastResynchronise; // millis() that we last dropped or gained time to resynchronise
const unsigned long TIME_BETWEEN_RESYNCHRONISE_S = 600; // once per ten minutes - should be enough

const long MAX_MISMATCH_SECONDS = 600; // if the mismatch is bigger than this, don't automatically resynch.

bool realTimeClockStatus;

void setupRTC(void){
  Wire.begin();
  realTimeClock.begin();
  currentTimeZoneSeconds = (long)getSetting(SET_timezone);
  timeMismatch = 0;
  timeOfLastResynchronise = millis();
  tickRTC();
}

void tickRTC() {
  bool success = refreshCurrentTime();
  if (success) {
    if ((millis() - timeOfLastResynchronise) >= TIME_BETWEEN_RESYNCHRONISE_S) {
      timeOfLastResynchronise = millis();
      tickResynchronise();
    }
  }
}

// returns true for success
bool refreshCurrentTime()
{
  realTimeClockStatus = realTimeClock.isrunning();
  if (realTimeClockStatus) {
    currentTimeUTC = realTimeClock.now();
    currentTimeWithZone = currentTimeUTC + TimeSpan(currentTimeZoneSeconds);
  }
  return realTimeClockStatus;
}

// print the time, no time zone
void printDateTimeNoCarret(Print &dest, DateTime displayTime) {
  dest.print(displayTime.year(), DEC);
  dest.print('/');
  dest.print(displayTime.month(), DEC);
  dest.print('/');
  dest.print(displayTime.day(), DEC);
  dest.print(' ');
  dest.print(displayTime.hour(), DEC);
  dest.print(':');
  dest.print(displayTime.minute(), DEC);
  dest.print(':');
  dest.print(displayTime.second(), DEC);
}

// print the time and time zone
void printDateTimeWithZone(Print &dest, DateTime dateTime) {
  printDateTimeNoCarret(dest, dateTime);
  dest.print(" UTC");
  dest.print(currentTimeZoneSeconds < 0 ? "-" : "+");
  int timezonehours = abs(currentTimeZoneSeconds) / 3600;
  int timezonemins = (abs(currentTimeZoneSeconds) / 60) % 60;
  dest.print(timezonehours);
  dest.print(":");
  dest.print(timezonemins/10);
  dest.print(timezonemins%10);
  dest.println();
}

// print the time, no time zone
void printDateTime(Print &dest, DateTime dateTime) {
  printDateTimeNoCarret(dest, dateTime);
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

// Parses a date & time with timezone
// "Dec 26 2009 12:34:56UTC+09:30" - must match this form and spacing and capitalisation exactly
// returns true for OK, false for a problem with the format
// retDateTime is the clock time in UTC+0:00, retTimeZone is the timezone in seconds eg +9:30 is 9.5 * 3600
bool parseDateTimeWithZone(const char newDateTimeWithZone[], DateTime &retDateTime, long &retTimeZone)
{
  if (strlen(newDateTimeWithZone) < DATETIMEFORMAT_TOTALLENGTH) return false;
  
  DateTime newRawTime(newDateTimeWithZone, newDateTimeWithZone + DATETIMEFORMAT_TIME_STARTPOS);

  long newTimeZone = 3600 * gettwodigit(newDateTimeWithZone + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4)
                             + 60 * gettwodigit(newDateTimeWithZone + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4 + 2 + 1);
  if (newDateTimeWithZone[DATETIMEFORMAT_TIMEZONE_STARTPOS + 3] == '-') {
    newTimeZone *= -1;
  }
  
  retDateTime = newRawTime - TimeSpan(newTimeZone);
  retTimeZone = newTimeZone;
  return true;
}

// "Dec 26 2009 12:34:56UTC+09:30"
// returns true for OK, false for a problem with the format
bool setDateTime(const char newDateTime[])
{
  DateTime newTime;
  long newTimeZone;
  bool success = parseDateWithTimeZone(newDateTime, newTime, newTimeZone);
  if (!success) return false;
  
  realTimeClock.adjust(newTime);
  currentTimeZoneSeconds = newTimeZone;
  setSetting(SET_timezone, currentTimeZoneSeconds);
  currentTimeUTC = realTimeClock.now();
  currentTimeWithZone = currentTimeUTC + TimeSpan(currentTimeZoneSeconds);

  return true;
}

const int DATETIMEFORMAT_DATELENGTH = 11; // "Dec 04 2018"
const int DATETIMEFORMAT_TIMELENGTH = 8; // "12:34:56"
const int DATETIMEFORMAT_TIMEZONE_LENGTH = 9; // "UTC+09:30"
const int DATETIMEFORMAT_TIME_STARTPOS = DATETIMEFORMAT_DATELENGTH + 1; // includes space
const int DATETIMEFORMAT_TIMEZONE_STARTPOS = DATETIMEFORMAT_TIME_STARTPOS + DATETIMEFORMAT_TIMELENGTH + 1; // includes space
const int DATETIMEFORMAT_TOTALLENGTH = DATETIMEFORMAT_TIMEZONE_STARTPOS + DATETIMEFORMAT_TIMEZONE_LENGTH;

// set the correct time, subsequent calls to tickResynchronise will gradually adjust the clock
//  to match this resynchronisation time
// returns the current clock error relative to the synch time (+ve --> arduino is ahead of synch time)
long setDateTimeForResynchronisation(unsigned long unixtimeseconds, long timezoneseconds) 
{
  currentTimeUTC = realTimeClock.now();
  timeZoneSeconds = newTimeZone;
  setSetting(SET_timezone, timeZoneSeconds);
  timeMismatch = currentTimeUTC - unixtimeseconds;
  return timeMismatch;
}

// resynchronise the clock (if required)
// resynchronisation is performed one second at a time
// this function should be called periodically (eg once per hour) to add or drop seconds
// it will wait up to a maximum of 1.1 second to make sure that the clock is aligned
RESYNCHstatus tickResynchronise()
{
  RESYNCHstatus resynchStatus = getResynchStatus();
  if (resynchStatus != RESYNCH_synchronising) {
    return resynchStatus;
  }
  unsigned long startmillis = millis();
  time_t currentClock = realTimeClock.now();
  time_t nowClock;
  while ((nowClock = realTimeClock.now()) <= currentClock)) {
     if (millis() - startmillis >= 1100) {        // abort if timeout
      return;
     }
  }
  realtimeClock.sync(nowClock + (timeMismatch < 0 ? 1 : -1));
  if (timeMismatch > 0) {
    --timeMismatch; 
  } else {
    ++timeMismatch;
  }
  return getResynchStatus();
}

RESYNCHstatus getResynchStatus()
{
  if (abs(timeMismatch) > MAX_MISMATCH_SECONDS) {
    return RESYNCH_mismatch_too_big;
  } else if (timeMismatch == 0) {
    return RESYNCH_not_required;
  } else {
    return RESYNCH_synchronising
  }
}
