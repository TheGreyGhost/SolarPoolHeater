#include "MovingAverage.h"
#include <float.h>

MovingAverage::MovingAverage(float alpha, int minSamples)
{
  m_alpha = alpha;
  m_minSamples = minSamples;
  clear();
}

void MovingAverage::addDatapoint(float value)
{
  if (m_numberOfPoints == 0) {
    m_ewma = value;
  } else {
    m_ewma = m_ewma * (1 - m_alpha) + value * m_alpha;
  }

  if (m_numberOfPoints < m_minSamples) {
    ++m_numberOfPoints;
  }
}

void MovingAverage::clear() { m_numberOfPoints = 0; }

float MovingAverage::getEWMA() { return m_ewma;}
    
bool MovingAverage::isValid() { return (m_numberOfPoints >= m_minSamples);}

