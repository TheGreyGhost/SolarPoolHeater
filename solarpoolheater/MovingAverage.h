/* Calculate exponential weighted moving average of a data stream
  
Usage:
  (1) construct a MovingAverage for each stream of data:
     a) specify the alpha ratio (weight of new samples)
     b) specify the spin-up samples (number of samples required before the result is valid)
  (2) call addDatapoint() each time a new datapoint is available
  (3) call clear() to reset 

  read the various stats using:
    getEWMA()
    isValid() - is the answer valid yet?	 
  */
#ifndef MovingAverage_h   // if x.h hasn't been included yet...
#define MovingAverage_h   //   #define this so the compiler knows it has been included
#include <Arduino.h>

class MovingAverage
{
  public:

	// alpha: the weight for each new sample[0 - 1], EWMA[n+1] = alpha * datapoint + (1 - alpha) * EWMA[n]
	// minsamples: the minimum number of samples received before the answer is valid
    MovingAverage(float alpha, int minSamples = 1);
		
    void addDatapoint(float value);
    void clear();

    float getEWMA();
    bool isValid(); 	

  private:
    int m_numberOfPoints;
    int m_minSamples;
    float m_ewma;
    float m_alpha;	
};

#endif
