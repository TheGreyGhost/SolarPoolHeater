#include <RTClib.h>
#include "RealTimeClock.h"

void printDateTime(DateTime dateTime);

RTC_DS1307 RTC;

void setupRTC(void){
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
}

void tickRTC() {
  if (Serial.available() > 0) {
  int instruct = Serial.read();
  switch (instruct) {
    case 'D': {
      DateTime now = RTC.now();
      printDateTime(now);
      break;
    } case 'S':
      RTC.set(RTC_MONTH, 6);
      RTC.set(RTC_HOUR, 16);
      break;
    }
  }
}

void printDateTime(DateTime dateTime) {
  Serial.print(dateTime.year(), DEC);
  Serial.print('/');
  Serial.print(dateTime.month(), DEC);
  Serial.print('/');
  Serial.print(dateTime.day(), DEC);
  Serial.print(' ');
  Serial.print(dateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(dateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(dateTime.second(), DEC);
  Serial.println();
}
