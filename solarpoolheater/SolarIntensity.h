#ifdef SOLARINTENSITY_H
#define SOLARINTENSITY_H

void setupSolarIntensity();
void tickSolarIntensity();

extern unsigned long cumulativeInsolation;
extern MovingAverage smoothedSolarIntensity(EWMA_ALPHA, EWMA_TIME_CONSTANT);

extern bool solarIntensityReadingInvalid;  
extern int lastInvalidReading;

#endif
