#include "DataStats.h"
#include <cfloat>

DataStats::DataStats()
{
  clear();
}

void DataStats::addDatapoint(float value)
{
  ++numberOfPoints;
  minValue = min(minValue, value);
  maxValue = max(maxValue, value);
  cumulativeSum += value;
}

void DataStats::clear()
{
  numberOfPoints = 0;
  minValue = FLT_MAX;
  maxValue = -FLT_MAX;
  cumulativeSum = 0;
}

unsigned long DataStats::getCount() {return numberOfPoints;}
float DataStats::getMin() {return minValue;}
float DataStats::getMax() {return maxValue;}
float DataStats::getAverage() {
  if (numberOfPoints == 0) return 0.0;
  return cumulativeSum / numberOfPoints;
}
