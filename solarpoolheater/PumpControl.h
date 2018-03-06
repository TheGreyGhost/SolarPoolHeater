#ifndef PUMPCONTROL_H
#define PUMPCONTROL_H

void setupPumpControl();
void tickPumpControl();

// the cumulative time that the pump has been running, in seconds.  resets at midnight.
extern float pumpRuntimeSeconds;
enum PumpState {PS_OFF_TIME=0x00, PS_OFF_REACHED_SETPOINT=0x01, PS_OFF_WAITING_FOR_SUN=0x02, PS_OFF_ERRORS=0x23, PS_OFF_EXCESSIVE_ERRORS=0x24, PS_OFF_OVERTEMP_HOT_INLET=0x25, PS_OFF_OVERTEMP_COLD_OUTLET=0x26, PS_ON=0x17, PS_ON_TIMING_OUT=0x18};
                  // pump on = bit 4 (16) set; error = bit 5 set.  Label index = bits 0 - 3
const int NUMBER_OF_PUMP_STATES = 9;

extern PumpState pumpState;

// returns a text description of the current pump state
const char *getCurrentPumpStateLabel();

// returns true if the pump is currently running
bool isPumpRunning();

// returns true if the pump control has an error
bool isPumpInError(); 

#endif
