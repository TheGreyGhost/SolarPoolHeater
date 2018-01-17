#ifndef TEMPERATURE_PROBES_H   
#define TEMPERATURE_PROBES_H  
#include <Arduino.h>

void setupTemperatureProbes();  // call to initialise the temperature probes 
void tickTemperatureProbes(); // call at frequent intervals (100 ms) to keep the probes up to date

#endif
