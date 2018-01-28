#include "RealTimeClock.h"
#include <Wire.h>

void printDateTime(DateTime dateTime);

RTC_DS1307 RTC;

void setupRTC(void){
  Wire.begin();
  RTC.begin();
}

void tickRTC() {
//  if (Serial.available() > 0) {
//  int instruct = Serial.read();
//  switch (instruct) {
//    case 'D': {
//      DateTime now = RTC.now();
//      printDateTime(now);
//      break;
//    } case 'S':
//      RTC.set(RTC_MONTH, 6);
//      RTC.set(RTC_HOUR, 16);
//      break;
//    }
//  }
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
