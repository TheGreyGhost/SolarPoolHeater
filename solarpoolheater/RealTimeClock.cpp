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

// "Dec 26 2009 12:34:56"
void setDateTime(const char newDateTime[])
{
  DateTime newTime(newDateTime, newDateTime + 12);
  realTimeClock.adjust(newTime);
}


