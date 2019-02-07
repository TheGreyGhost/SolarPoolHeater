#ifndef PUMPCONTROL_H
#define PUMPCONTROL_H
#include "DataStats.h"
#include "RealTimeClock.h"

void setupPumpControl();
void tickPumpControl();

// the cumulative time that the pump has been running, in seconds.  resets at midnight.
extern float pumpRuntimeSeconds;
enum PumpState {PS_OFF_TIME=0x00, PS_OFF_REACHED_SETPOINT=0x01, PS_OFF_WAITING_FOR_SUN=0x02, PS_OFF_ERRORS=0x23, PS_OFF_EXCESSIVE_ERRORS=0x24, PS_OFF_OVERTEMP_HOT_INLET=0x25, PS_OFF_OVERTEMP_COLD_OUTLET=0x26, PS_ON=0x17, PS_ON_TIMING_OUT=0x18,
                PS_OFF_DISABLED=0x09, PS_OFF_SURGE_TANK_LEVEL_LOW=0x2a, PS_OFF_PUMP_CYCLING=0x2b};
                  // pump on = bit 4 (16) set; error = bit 5 set.  Label index = bits 0 - 3
const int NUMBER_OF_PUMP_STATES = 12;

extern PumpState pumpState;

// returns a text description of the current pump state
const char *getCurrentPumpStateLabel();

// return the current state of the pump
PumpState getPumpState(); 

// returns true if the pump is currently running
bool isPumpRunning();

// returns true if the pump control has an error
bool isPumpInError(); 

// returns true if the system is idle (controls logging frequency)
bool isSystemIdle();

// returns the current pump error code (or 0 if no error)
byte getPumpErrorCode();

// resets any pump errors
void resetPumpErrors();

extern bool surgeTankLevelOK;  // if false, the surge tank level switch is off (level is low)
extern DataStats surgeTankLevelStats; 

extern float lastSampledPoolTemperature;
extern DateTime lastSamplePoolTemperatureTime;
extern bool lastSampledPoolTemperatureIsValid;

#endif
