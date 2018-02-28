#ifndef SOLARINTENSITY_H
#define SOLARINTENSITY_H
#include "MovingAverage.h"

void setupSolarIntensity();
void tickSolarIntensity();

extern float cumulativeInsolation;
extern MovingAverage smoothedSolarIntensity;

extern bool solarIntensityReadingInvalid;  
extern int lastInvalidReading;

#endif
