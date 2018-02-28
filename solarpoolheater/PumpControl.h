#ifndef PUMPCONTROL_H
#define PUMPCONTROL_H

void setupPumpControl();
void tickPumpControl();

// the cumulative time that the pump has been running, in seconds.  resets at midnight.
extern float pumpRuntimeSeconds;

#endif
