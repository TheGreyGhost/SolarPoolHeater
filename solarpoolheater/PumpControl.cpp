#include <Arduino.h>
#include "PumpControl.h"
#include "RealTimeClock.h"
#include "SystemStatus.h"
#include "TemperatureProbes.h"
#include "SolarIntensity.h"
#include "Settings.h"
#include "Simulate.h"

// rules:
// 1) If time is before pump start time, or after pump stop time, turn off
// 2) If there are system errors, do not run pump.  If system errors show up too many times in one day, shut off permanently
// 2b) If the surge tank level goes low, permanently shut off
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
// 3) temperature setpoint & hysteresis
// 4) insolation threshold
// 5) pumprunout time (time to keep running after conditions are unfavourable.
// 6) minimum diff hot inlet - cold inlet to keep pump running
// 7) hot inlet overtemperature
// 8) cold outlet overtemperature

bool surgeTankLevelOK;
const int SURGE_TANK_LEVEL_PIN = 4;

bool pumpIsRunning;
PumpState pumpState;
int systemErrorCountToday;
unsigned long timeoutStartMillis;
float pumpRuntimeSeconds;

unsigned long lastMillisPC;
uint8_t lastDayPC;
const int SAMPLE_PERIOD_MS = 1000;

const int PUMP_PIN = 3;

const char* pumpStateLabels[NUMBER_OF_PUMP_STATES] = {"OFF(TIME)", "OFF(>SETPT)", "OFF(NO_SUN)", "OFF(ERRORS)", "OFF(TOO_MANY_ERRORS)", "OFF(OVERTEMP_HOT_INLET)", 
                                                      "OFF(OVERTEMP_COLD_OUTLET)", "ON", "ON(TIMING_OUT)", "OFF(DISABLED)", "OFF(SURGE_TANK_LEVEL_LOW)", "OFF(EXCESSIVE_PUMP_CYCLING)"};

int pumpTurnOnCount;
unsigned long firstPumpTurnOnMillis;

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

byte getPumpErrorCode()
{
  if (!isPumpInError()) return 0;
  return (byte)(pumpState & 0x0f);
}

PumpState getPumpState()
{
  return pumpState;
}

void setupPumpControl()
{
  pinMode(SURGE_TANK_LEVEL_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, OUTPUT);

  pumpRuntimeSeconds = 0;
  pumpIsRunning = false;
  pumpState = PS_OFF_WAITING_FOR_SUN;
  lastDayPC = currentTime.day();    
  systemErrorCountToday = 0;
  pumpTurnOnCount = 0;
}

PumpState checkForPumpStateTransition(float currentHours, unsigned long timeNow);

void tickPumpControl()
{
  surgeTankLevelOK = (digitalRead(SURGE_TANK_LEVEL_PIN) == HIGH);
  if (isBeingSimulated(SIM_SURGE_TANK_LEVEL)) {
    surgeTankLevelOK = fabs(getSimulatedValue(SIM_SURGE_TANK_LEVEL, 0)) >= 0.5;
  }
  
  bool initialPumpIsRunning = pumpIsRunning;
  unsigned long timeNow = millis();
  if (timeNow - lastMillisPC < SAMPLE_PERIOD_MS) return;

  if (pumpIsRunning) {
    pumpRuntimeSeconds += (timeNow - lastMillisPC) / 1000.0;
  }  

  if (currentTime.day() != lastDayPC) {
    lastDayPC = currentTime.day();
    pumpRuntimeSeconds = 0;
    systemErrorCountToday = 0;
    pumpTurnOnCount = 0;
  }

  float currentHours = currentTime.hour() + currentTime.minute() / 60.0;

  switch (pumpState) {
    case PS_OFF_PUMP_CYCLING:
    case PS_OFF_SURGE_TANK_LEVEL_LOW:
    case PS_OFF_EXCESSIVE_ERRORS:
    case PS_OFF_OVERTEMP_HOT_INLET:
    case PS_OFF_OVERTEMP_COLD_OUTLET: {
      // permanently off, can't change state
      break;
    }

    case PS_OFF_ERRORS: {
      if (systemErrorCountToday >=  getSetting(SET_maxDailySystemErrorCount)) {
        pumpState = PS_OFF_EXCESSIVE_ERRORS;
      } else if (!shutdownErrorsPresent()) {
        pumpState = PS_OFF_WAITING_FOR_SUN;
        ++systemErrorCountToday;
      }  
      break;  
    }

    case PS_OFF_DISABLED:
    case PS_OFF_TIME:
    case PS_OFF_REACHED_SETPOINT:
    case PS_OFF_WAITING_FOR_SUN:
    case PS_ON:
    case PS_ON_TIMING_OUT: {
      pumpState = checkForPumpStateTransition(currentHours, timeNow);
      break;
    }

    default: {
      assertFailureCode = ASSERT_INVALID_SWITCH;
      console->print("Invalid Switch tickPumpControl::pumpState:");
      console->println(pumpState);
      pumpState = PS_OFF_EXCESSIVE_ERRORS;
    }
  }

  pumpIsRunning = isPumpRunning();

  digitalWrite(PUMP_PIN, pumpIsRunning ? HIGH : LOW);

  lastMillisPC = timeNow;

  // check to see if pump is turning on and off too often (logic error somewhere) --> if so, disable it to prevent damage
  if (pumpIsRunning && !initialPumpIsRunning) {
    if (pumpTurnOnCount == 0) {
      firstPumpTurnOnMillis = timeNow;
    } 
    ++pumpTurnOnCount;
    if (pumpTurnOnCount >= 4) {
      pumpTurnOnCount = 0;
      if (timeNow - firstPumpTurnOnMillis < getSetting(SET_minSecondsPerFourPumpOns) * 1000) {
        pumpState = PS_OFF_PUMP_CYCLING;              
      }
    }
  }
  
}

PumpState checkForPumpStateTransition(float currentHours, unsigned long timeNow)
{
  // first look for conditions which always turn the pump off

  if (shutdownErrorsPresent()) {
    return PS_OFF_ERRORS;
  } else if (!surgeTankLevelOK) {
    return PS_OFF_SURGE_TANK_LEVEL_LOW;
  } else if (fabs(getSetting(SET_dontRunPump)) > 0.5) {
    return PS_OFF_DISABLED;
  } else if (currentHours < getSetting(SET_onTimeHours) || currentHours > getSetting(SET_offTimeHours)) {
    return PS_OFF_TIME;
  } else if (temperatureDataStats[HX_COLD_OUTLET].getCount() >= 1 && 
             temperatureDataStats[HX_COLD_OUTLET].getMax() > getSetting(SET_coldOutletAlarm)) {
    return PS_OFF_OVERTEMP_COLD_OUTLET;
  } else if (temperatureDataStats[HX_HOT_INLET].getCount() >= 1 && 
             temperatureDataStats[HX_HOT_INLET].getMax() > getSetting(SET_hotInletAlarm)) {
    return PS_OFF_OVERTEMP_HOT_INLET;
  } else if (!smoothedTemperatures[HX_COLD_INLET].isValid()) {
    return PS_OFF_WAITING_FOR_SUN;            
  } else if (smoothedTemperatures[HX_COLD_INLET].getEWMA() > 
               getSetting(SET_temperatureSetpoint) - (pumpState == PS_OFF_REACHED_SETPOINT ? getSetting(SET_temperatureSetpointHysteresis) : 0)) {
    return PS_OFF_REACHED_SETPOINT;            
  } 

  // look for other conditions which might cause a transition

  bool sunIsShining = !solarIntensityReadingInvalid && smoothedSolarIntensity.isValid() &&
                      smoothedSolarIntensity.getEWMA() >= getSetting(SET_solarIntensityThreshold);
  bool heatAvailable = smoothedTemperatures[HX_HOT_INLET].getEWMA() - smoothedTemperatures[HX_COLD_INLET].getEWMA() 
                         >= getSetting(SET_minimumHotInletMinusColdInlet);
  switch (pumpState) {
    case PS_OFF_DISABLED:
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
      if (!sunIsShining && !heatAvailable && (timeNow - timeoutStartMillis) > getSetting(SET_belowMinimumTimeoutSeconds) * 1000) {
        return PS_OFF_WAITING_FOR_SUN;
      }
      break;
    }  
     
    default: {
      assertFailureCode = ASSERT_INVALID_SWITCH;
      console->print("Invalid Switch checkForPumpStateTransition::pumpState:");
      console->println(pumpState);
      return PS_OFF_EXCESSIVE_ERRORS;
    }
  }  
  return pumpState;
}

