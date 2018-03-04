#include <Arduino.h>
#include "PumpControl.h"
#include "RealTimeClock.h"
#include "SystemStatus.h"
#include "TemperatureProbes.h"

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

const float onTimeHours = 8.0;             // before this time (hours), turn off
const float offTimeHours = 19.0;           // after this time (hours), turn off
const float temperatureSetpoint = 28.0;  // target pool temperature
const float solarIntensityThreshold = 20.0;             // ignore solar intensity below this threshold 
const float solarInsolationThreshold = 20.0 * 10 * 60;  // if we receive this much insolation, start the pump
const float insolationTriggerRunSeconds = 180.0;        // time to run the pump once insolation trigger reached, before watching HotInlet minus ColdInlet
const float minimumHotInletMinusColdInlet = 2.0;        // minimum difference between hot inlet and cold inlet to keep running pump
const float belowMinimumTimeoutSeconds = 180.0;         // once the deltaT is below minimumHotInletMinusColdInlet, run for this timeout then stop

const float hotInletAlarm = 60.0;
const float coldOutletAlarm = 45.0; 

bool pumpIsRunning;
bool permanentShutdown;

float pumpRuntimeSeconds;

enum PumpState {PS_OFF_TIME, PS_OFF_ERRORS, PS_OFF_REACHED_SETPOINT, PS_OFF_WAITING_FOR_SUN, PS_ON, PS_ON_TIME};

unsigned long lastMillisPC;
uint8_t lastDayPC;
const int SAMPLE_PERIOD_MS = 1000;

const int PUMP_PIN = 3;

void setupPumpControl()
{
  pinMode(PUMP_PIN, OUTPUT);

  pumpRuntimeSeconds = 0;
  pumpIsRunning = false;
  permanentShutdown = false;
  lastDayPC = currentTime.day();    
}

void tickPumpControl()
{
  unsigned long timeNow = millis();
  if (timeNow - lastMillisPC < SAMPLE_PERIOD_MS) return;

  if (pumpIsRunning) {
    pumpRuntimeSeconds += (timeNow - lastMillisPC) / 1000.0;
  }  

  if (currentTime.day() != lastDayPC) {
    lastDayPC = currentTime.day();
    pumpRuntimeSeconds = 0;
  }

  if (shutdownErrorsPresent || permanentShutdown) {
    pumpIsRunning = false;
  } else if (currentTime.hour() < onTimeHours) {
    pumpIsRunning = false;
  } else if (currentTime.hour() > offTimeHours) {
    pumpIsRunning = false;
  } else if (!smoothedTemperatures[HX_COLD_INLET].isValid() || 
              smoothedTemperatures[HX_COLD_INLET].getEWMA() > temperatureSetpoint) {
    pumpIsRunning = false;              
    permanentShutdown = true;
  } else if (temperatureDataStats[HX_COLD_OUTLET].getCount() >= 1 && 
             temperatureDataStats[HX_COLD_OUTLET].getMax() > coldOutletAlarm) {
    pumpIsRunning = false;              
    permanentShutdown = true;
  } else if (temperatureDataStats[HX_HOT_INLET].getCount() >= 1 && 
             temperatureDataStats[HX_HOT_INLET].getMax() > hotInletAlarm) {
    pumpIsRunning = false;              
    permanentShutdown = true;
  } else {
    pumpIsRunning = true;
  }

  digitalWrite(PUMP_PIN, pumpIsRunning ? HIGH : LOW);

  lastMillisPC = timeNow;
}

