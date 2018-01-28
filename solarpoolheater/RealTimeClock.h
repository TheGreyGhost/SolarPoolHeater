#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H
#include <Arduino.h>
#include <RTClib.h>

void setupRTC();
void tickRTC();

void printDateTime(Print &dest, DateTime dateTime);

#endif
