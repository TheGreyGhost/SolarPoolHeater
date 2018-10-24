#ifndef SOLARINTENSITY_H
#define SOLARINTENSITY_H
#include "GlobalDefines.h"
#include "MovingAverage.h"

void setupSolarIntensity();
void tickSolarIntensity();

extern float cumulativeInsolation;
extern MovingAverage smoothedSolarIntensity;

extern bool solarIntensityReadingInvalid;  
extern int lastInvalidReading;

const float SOLAR_INTENSITY_FULL = 1000.0;
const float SOLAR_INTENSITY_10pc = SOLAR_INTENSITY_FULL * 0.1;
#endif
