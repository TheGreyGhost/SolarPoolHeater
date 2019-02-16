#include "RealTimeClock.h"
#include <Wire.h>
#include "Settings.h"
#include "SystemStatus.h"

RTC_DS1307 realTimeClock;
DateTime currentTimeUTC;
DateTime currentTimeWithZone;
long currentTimeZoneSeconds;  // eg UTC+9:30 is 9.5 * 3600.  Also saved in EEPROM.

long timeMismatch;  // seconds that the RTC is ahead of the true time
unsigned long timeOfLastResynchronise; // millis() of the time we last performed a resynch (i.e. that we last dropped or gained time)
const unsigned long TIME_BETWEEN_RESYNCHRONISE_S = 600; // once per ten minutes - should be enough

const long MAX_MISMATCH_SECONDS = 600; // if the mismatch is bigger than this, don't automatically resynch.

const long TIMEZONE_MAX = 12 * 3600L;
const long TIMEZONE_MIN = -12 * 3600L;
const long TIMEZONE_DEFAULT = 9.5 * 3600L;

bool realTimeClockStatus;

void setupRTC(void){
  Wire.begin();
  realTimeClock.begin();
  currentTimeZoneSeconds = (long)getSetting(SET_timezone);
  if (currentTimeZoneSeconds > TIMEZONE_MAX || currentTimeZoneSeconds < TIMEZONE_MIN) {
    currentTimeZoneSeconds = TIMEZONE_DEFAULT;
    assertFailureCode = ASSERT_INVALID_TIMEZONE;
  }
  
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

// prints a two digit number with a leading zero if required
void printTwoDigit(Print &dest, uint8_t number) 
{
  if (number < 10) dest.print("0");
  dest.print(number, DEC);
}

// print the time, no time zone
void printDateTimeNoCarret(Print &dest, DateTime displayTime) {
  dest.print(displayTime.year(), DEC);
  dest.print('/');
  printTwoDigit(dest, displayTime.month());
  dest.print('/');
  printTwoDigit(dest, displayTime.day());
  dest.print(' ');
  printTwoDigit(dest, displayTime.hour());
  dest.print(':');
  printTwoDigit(dest, displayTime.minute());
  dest.print(':');
  printTwoDigit(dest, displayTime.second());
}

void printDateTimeWithZone(Print &dest, DateTime dateTime, long timeZone)
{
  printDateTimeNoCarret(dest, dateTime);
  dest.print(" UTC");
  dest.print(timeZone < 0 ? "-" : "+");
  long timezonehours = abs(timeZone) / 3600;
  long timezonemins = (abs(timeZone) / 60) % 60;
  printTwoDigit(dest, (uint8_t)timezonehours);
  dest.print(":");
  printTwoDigit(dest, (uint8_t)timezonemins);
  dest.println();
}

// print the time and time zone
void printDateTimeWithZone(Print &dest, DateTime dateTime) 
{
  printDateTimeWithZone(dest, dateTime, currentTimeZoneSeconds);
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

  long newTimeZone = 3600L * gettwodigit(newDateTimeWithZone + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4)
                             + 60L * gettwodigit(newDateTimeWithZone + DATETIMEFORMAT_TIMEZONE_STARTPOS + 4 + 2 + 1);
  if (newDateTimeWithZone[DATETIMEFORMAT_TIMEZONE_STARTPOS + 3] == '-') {
    newTimeZone *= -1;
  }
  if (newTimeZone > TIMEZONE_MAX || newTimeZone < TIMEZONE_MIN) {
    return false;
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
  bool success = parseDateTimeWithZone(newDateTime, newTime, newTimeZone);
  if (success) {
    success = setDateTime(newTime, newTimeZone);
  }
  return success;
}

// set the current date + time 
// retDateTime is the clock time in UTC+0:00, retTimeZone is the timezone in seconds eg +9:30 is 9.5 * 3600
bool setDateTime(DateTime newTimeUTC, long newTimezoneSeconds)
{
  if (newTimezoneSeconds > TIMEZONE_MAX || newTimezoneSeconds < TIMEZONE_MIN) {
    return false;
  }
  
  realTimeClock.adjust(newTimeUTC);
  currentTimeZoneSeconds = newTimezoneSeconds;
  setSetting(SET_timezone, currentTimeZoneSeconds);
  currentTimeUTC = realTimeClock.now();
  currentTimeWithZone = currentTimeUTC + TimeSpan(currentTimeZoneSeconds);

  return true;
}

// set the correct time, subsequent calls to tickResynchronise will gradually adjust the clock
//  to match this resynchronisation time
// returns the current clock error relative to the synch time (+ve --> arduino is ahead of synch time)
long setDateTimeForResynchronisation(unsigned long unixtimeseconds, long timezoneseconds) 
{
  currentTimeUTC = realTimeClock.now();
  currentTimeZoneSeconds = timezoneseconds;
  setSetting(SET_timezone, timezoneseconds);
  timeMismatch = currentTimeUTC.unixtime() - unixtimeseconds;
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
  uint32_t currentClock = realTimeClock.now().unixtime();
  uint32_t nowClock;
  while ((nowClock = realTimeClock.now().unixtime()) <= currentClock) {
     if (millis() - startmillis >= 1100) {        // abort if timeout
      return RESYNCH_error;
     }
  }
  realTimeClock.adjust(DateTime(nowClock + (timeMismatch < 0 ? 1 : -1)));
  if (timeMismatch > 0) {
    --timeMismatch; 
  } else {
    ++timeMismatch;
  }
  return getResynchStatus();
}

RESYNCHstatus getResynchStatus()
{
  if (!realTimeClockStatus) return RESYNCH_error;
  if (abs(timeMismatch) > MAX_MISMATCH_SECONDS) {
    return RESYNCH_mismatch_too_big;
  } else if (timeMismatch == 0) {
    return RESYNCH_not_required;
  } else {
    return RESYNCH_synchronising;
  }
}

const char* resynchStatusLabels[NUMBER_OF_RESYNCH_STATES] = {"synchronised", "synchronising", "mismatch too big", "error"};

const char *getResynchStatusLabel()
{
  int labelIdx = getResynchStatus();
  if (labelIdx < 0 || labelIdx >= NUMBER_OF_RESYNCH_STATES) {
    return "INVALID RTC RESYNCH STATE";
  }
  return resynchStatusLabels[labelIdx];
}

long getSynchronisationMismatch()
{
  return timeMismatch;
}
