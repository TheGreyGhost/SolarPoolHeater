#include <Arduino.h>
#include "PumpControl.h"
#include "RealTimeClock.h"
#include "SystemStatus.h"
#include "TemperatureProbes.h"

// rules:
// 1) If time is before pump start time, or after pump stop time, turn off
// 2) If there are system errors, do not run pump.  If system errors show up multiple times in one day, shut off permanently
// 3) if cold inlet is above target, do not run pump
// 4) if the solar insolation is above a threshold, start the pump or keep it running.
// 5) If the pump is running and the hot inlet is warmer than the cold inlet by at least threshold, keep running.  Otherwise, run for X minutes then stop.

// detect error conditions:
//   hot inlet > alarm
//   cold outlet > alarm
// Once an overtemp condition has occurred, shut off the system permanently until reset

// maybe later:
  // (hot in - hot out) vs (cold out - cold in) vs (ave hot - ave cold) mismatched:
  //    either cold pump is off, or hot pump is not working

//save settings in EEPROM:
// 1) on clock time
// 2) off clock time
// 3) temperature setpoint
// 4) insolation threshold
// 5) pumprunout time (time to keep running after conditions are unfavourable.

const float onTimeHours = 9.0;             // before this time (hours), turn off
const float offTimeHours = 19.0;           // after this time (hours), turn off
const float temperatureSetpoint = 28.0;  // target pool temperature
const float temperatureSetpointHysteresis = 1.0; // once target temp setpoint is reached, stay off until below setpoint - hysteresis
const float solarIntensityThreshold = SOLAR_INTENSITY_10pc;             // start the pump once the solar intensity is above this threshold 
//const float solarInsolationThreshold = 20.0 * 10 * 60;  // if we receive this much insolation, start the pump
//const float insolationTriggerRunSeconds = 180.0;        // time to run the pump once insolation trigger reached, before watching HotInlet minus ColdInlet
const float minimumHotInletMinusColdInlet = 1.0;        // minimum difference between hot inlet and cold inlet to keep running pump
const float belowMinimumTimeoutSeconds = 180.0;         // once the deltaT is below minimumHotInletMinusColdInlet, run for this timeout then stop
const int maxDailySystemErrorCount = 5;  // if we get more than this number of system errors in one day, stop the pump permanently.
const float hotInletAlarm = 60.0;
const float coldOutletAlarm = 45.0; 


bool pumpIsRunning;
PumpState pumpState;
int systemErrorCountToday;
unsigned long timeoutStartMillis;
float pumpRuntimeSeconds;

unsigned long lastMillisPC;
uint8_t lastDayPC;
const int SAMPLE_PERIOD_MS = 1000;

const int PUMP_PIN = 3;

const char* pumpStateLabels[NUMBER_OF_PUMP_STATES] = {};

// returns a text description of the current pump state
const char *getCurrentPumpStateLabel()
{
  int labelIdx = pumpState & 0x0f;
  if (labelIdx >= NUMBER_OF_PUMP_STATES) {
    return "INVALID PUMP STATE";
  }
  return pumpStateLabels[labelIdx];
}

// returns true if the pump is currently running
bool isPumpRunning() 
{
  return (pumpState & 0x10);
}

// returns true if the pump control has an error
bool isPumpInError()
{
  return (pumpState & 0x20);
}

void setupPumpControl()
{
  pinMode(PUMP_PIN, OUTPUT);

  pumpRuntimeSeconds = 0;
  pumpIsRunning = false;
  pumpState = PS_OFF_WAITING_FOR_SUN;
  lastDayPC = currentTime.day();    
  systemErrorCountToday = 0;
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
    systemErrorCountToday = 0;
  }

  float currentHours = currentTime.hour() + currentTime.day() / 60.0;

enum PumpState {PS_OFF_TIME=0x00, PS_OFF_REACHED_SETPOINT=0x01, PS_OFF_WAITING_FOR_SUN=0x02, PS_OFF_ERRORS=0x23, PS_OFF_EXCESSIVE_ERRORS=0x24, PS_OFF_OVERTEMP_HOT_INLET=0x25, PS_OFF_OVERTEMP_COLD_OUTLET=0x26, PS_ON=0x17, PS_ON_TIMING_OUT=0x18};

  switch (pumpState) {
    case PS_OFF_EXCESSIVE_ERRORS:
    case PS_OFF_OVERTEMP_HOT_INLET:
    case PS_OFF_OVERTEMP_COLD_OUTLET: {
      // permanently off, can't change state
      break;
    }

    case PS_OFF_ERRORS: {
      if (systemErrorCountToday >=  maxDailySystemErrorCount) {
        pumpState = PS_OFF_EXCESSIVE_ERRORS;
      }
      if (!shutdownErrorsPresent()) {
        pumpState = PS_OFF_WAITING_FOR_SUN;
        ++systemErrorCountToday;
      }  
      break;  
    }
    
    case PS_OFF_TIME:
    case PS_OFF_REACHED_SETPOINT:
    case PS_OFF_WAITING_FOR_SUN:
    case PS_ON:
    case PS_ON_TIMING_OUT: {
      pumpState = checkForPumpStateTransition();
      break;
    }

    default: {
      assertFailureCode = ASSERT_INVALID_SWITCH;
      pumpState = PS_OFF_EXCESSIVE_ERRORS;
    }
  }

  pumpIsRunning = isPumpRunning();

  digitalWrite(PUMP_PIN, pumpIsRunning ? HIGH : LOW);

  lastMillisPC = timeNow;
}

PumpState checkForPumpStateTransition()
{
  // look for conditions which always turn the pump off
  if (shutdownErrorsPresent()) {
    return PS_OFF_ERRORS;
  } else if (currentHours < onTimeHours || currentHours > offTimeHours) {
    return PS_OFF_TIME;
  } else if (temperatureDataStats[HX_COLD_OUTLET].getCount() >= 1 && 
             temperatureDataStats[HX_COLD_OUTLET].getMax() > coldOutletAlarm) {
    return PS_OFF_OVERTEMP_COLD_OUTLET;
  } else if (temperatureDataStats[HX_HOT_INLET].getCount() >= 1 && 
             temperatureDataStats[HX_HOT_INLET].getMax() > hotInletAlarm) {
    return PS_OFF_OVERTEMP_HOT_INLET;
  } else if (!smoothedTemperatures[HX_COLD_INLET].isValid()) {
    return PS_OFF_WAITING_FOR_SUN;            
  } else if (smoothedTemperatures[HX_COLD_INLET].getEWMA() > 
               temperatureSetpoint - (pumpState == PS_OFF_REACHED_SETPOINT ? temperatureSetpointHysteresis : 0)) {
    return PS_OFF_REACHED_SETPOINT;            
  } 

  // look for other conditions
  switch (pumpState) {
    bool sunIsShining = !solarIntensityReadingInvalid && smoothedSolarIntensity.isValid() &&
                        smoothedSolarIntensity.getEWMA() >= solarIntensityThreshold;
    bool heatAvailable = smoothedTemperatures[HX_HOT_INLET].getEWMA() - smoothedTemperatures[HX_COLD_INLET].getEWMA()) 
                           >= minimumHotInletMinusColdInlet;
    case PS_OFF_REACHED_SETPOINT:
    case PS_OFF_TIME: {
      return PS_OFF_WAITING_FOR_SUN;
      break;
    }
    case PS_OFF_WAITING_FOR_SUN: {
      if (sunIsShining) {
        return PS_ON;    
      }
      break;
    }
    case PS_ON: {
      if (!sunIsShining && !heatAvailable) {
        timeoutStartMillis = timeNow;
        return PS_ON_TIMING_OUT;
      }  
      break;
    }
    case PS_ON_TIMING_OUT: {
      if (!sunIsShining && !heatAvailable && (timeNow - timeoutStartMillis) > belowMinimumTimeoutSeconds)) {
        return PS_OFF_WAITING_FOR_SUN;
      }
      break;
    }  
     
    default: {
      assertFailureCode = ASSERT_INVALID_SWITCH;
      return PS_OFF_EXCESSIVE_ERRORS;
    }
  }  
  return pumpState;
}

