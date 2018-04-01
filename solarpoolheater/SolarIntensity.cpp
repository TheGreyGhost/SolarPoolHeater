#include <Arduino.h>
#include "SolarIntensity.h"
#include "RealTimeClock.h"
#include "Simulate.h"

const int SOLAR_INTENSITY_PIN = A4;

const float EWMA_TIME_CONSTANT = 120.0; // decay time in seconds (time to drop to 1/e) assuming 1 Hz sampling frequency
const float EWMA_ALPHA = 1.0/120.0;  // corresponds roughly to 120 seconds decay time assuming 1 Hz sampling frequency

float cumulativeInsolation;
MovingAverage smoothedSolarIntensity(EWMA_ALPHA, EWMA_TIME_CONSTANT);

bool solarIntensityReadingInvalid;  
int lastInvalidReading;
unsigned long lastMillis;
uint8_t lastDay;

void setupSolarIntensity()
{
  analogReference(INTERNAL1V1);
  cumulativeInsolation = 0;
  lastMillis = 0; 
  lastDay = currentTime.day();
}

const int SAMPLE_PERIOD_MS = 1000;

  // expect the maximum reading to be about 1000 mV, the analog ref is 1.1V, so the max reading is nearly 1024

const int MINIMUM_VALID_READING = 0;
const int MAXIMUM_VALID_READING = 1000;

// update smoothed intensity and total daily insolation (reset at midnight)
void tickSolarIntensity()
{
  unsigned long timeNow = millis();
  if (timeNow - lastMillis < SAMPLE_PERIOD_MS) return;
  
  int value = analogRead(SOLAR_INTENSITY_PIN);
  value = (int)getSimulatedValue(SIM_SOLAR_INTENSITY, value);
  
  solarIntensityReadingInvalid = false;
  if (value < MINIMUM_VALID_READING) {
    solarIntensityReadingInvalid = true;
    lastInvalidReading = value;
    value = MINIMUM_VALID_READING;
  } else if (value > MAXIMUM_VALID_READING) {
    solarIntensityReadingInvalid = true;
    lastInvalidReading = value;
    value = MAXIMUM_VALID_READING;
  }
  cumulativeInsolation += value / 1000.0 * (timeNow - lastMillis);
  smoothedSolarIntensity.addDatapoint(value);

  if (currentTime.day() != lastDay) {
    lastDay = currentTime.day();
    cumulativeInsolation = 0;
  }
  lastMillis = timeNow;
}

