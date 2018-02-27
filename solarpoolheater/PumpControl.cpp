#include "PumpControl.h"

// rules:
// 1) If time is before pump start time, or after pump stop time, turn off
// 2) If there are system errors, do not run pump.
// 2b) if cold inlet is above target, do not run pump
// 3) if the solar insolation is above a threshold, start the pump and run for X minutes
// 4) If the hot inlet is warmer than the cold inlet by at least threshold, keep running.  Otherwise, run for X minutes then stop.

// detect error conditions:
// hot inlet > alarm
// cold outlet > alarm
// (hot in - hot out) vs (cold out - cold in) vs (ave hot - ave cold) mismatched:
//    either cold pump is off, or hot pump is not working

//save settings in EEPROM:
// 1) on clock time
// 2) off clock time
// 3) temperature setpoint
// 4) insolation threshold
// 5) pumprunout time (time to keep running after conditions are unfavourable.

bool pumpIsRunning;

float pumpRuntimeSeconds;

unsigned long lastMillis;
uint8_t lastDay;
const int SAMPLE_PERIOD_MS = 1000;

const int PUMP_PIN = 3;

void setupPumpControl()
{
  pinMode(PUMP_PIN, OUTPUT);

  pumpRuntimeSeconds = 0;
  pumpIsRunning = false;
  lastDay = currentTime.day();    
}

void tickPumpControl()
{
  unsigned long timeNow = millis();
  if (timeNow - lastMillis < SAMPLE_PERIOD_MS) return;

  if (pumpIsRunning) {
    pumpRuntimeSeconds += (timeNow - lastMillis) / 1000.0;
  }  

  if (currentTime.day() != lastDay) {
    lastDay = currentTime.day();
    pumpRuntimeSeconds = 0;
  }

  digitalWrite(PUMP_PIN, pumpIsRunning ? HIGH : LOW);

  lastMillis = timeNow;
}

