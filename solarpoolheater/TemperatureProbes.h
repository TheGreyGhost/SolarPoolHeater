#ifndef TEMPERATURE_PROBES_H   
#define TEMPERATURE_PROBES_H  
#include <Arduino.h>
#include "MovingAverage.h"
#include "DataStats.h"
#include "SystemStatus.h"

void setupTemperatureProbes();  // call to initialise the temperature probes 
void tickTemperatureProbes(); // call at frequent intervals (100 ms) to keep the probes up to date

extern bool echoProbeReadings;

const bool DEBUG_TEMP = false;

const int HX_HOT_INLET = 0;
const int HX_HOT_OUTLET = 1;
const int HX_COLD_INLET = 2;
const int HX_COLD_OUTLET = 3;
const int LAST_PROBE_IDX = HX_COLD_OUTLET;
const int NUMBER_OF_PROBES = LAST_PROBE_IDX + 1;

extern const char* probeNames[NUMBER_OF_PROBES];

enum ProbeStatus {PS_OK, PS_NOT_FOUND, PS_BUS_FAILURE, PS_CRC_FAILURE, PS_IMPLAUSIBLE_VALUE};
extern volatile enum ProbeStatus probeStatuses[NUMBER_OF_PROBES];

extern unsigned long errorCountBusFailure;
extern unsigned long errorCountNotFound[NUMBER_OF_PROBES];
extern unsigned long errorCountCRCFailure[NUMBER_OF_PROBES];
extern unsigned long errorCountImplausibleValue[NUMBER_OF_PROBES];
extern int16_t errorLastImplausibleValueRaw[NUMBER_OF_PROBES];
extern float errorLastImplausibleValueC[NUMBER_OF_PROBES];

extern MovingAverage smoothedTemperatures[NUMBER_OF_PROBES];
                  
extern DataStats temperatureDataStats[NUMBER_OF_PROBES];


#endif
