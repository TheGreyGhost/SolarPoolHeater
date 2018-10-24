/* Calculate simple data statistics for data logging 
  
Usage:
  (1) construct a DataStats for each stream of data
  (2) call addDatapoint() each time a new datapoint is available
  (3) call clear() to reset the statistics

  read the various stats using:
    getCount, getMin, getMax, getAverage	 
 
  Note - the average will become inaccurate for large numbers of datapoints due to precision loss
    (the accuracy is approx = 6 digits minus the number of digits in the datapoint count)		
  */
#ifndef DataStats_h   // if x.h hasn't been included yet...
#define DataStats_h   //   #define this so the compiler knows it has been included
#include "GlobalDefines.h"
//#include <Arduino.h>

class DataStats
{
  public:
    DataStats();
    void addDatapoint(float value);
    void clear();

    unsigned long getCount();	
    float getMin();
    float getMax();
    float getAverage();    

  private:
    unsigned long numberOfPoints;
    float minValue;
    float maxValue;
    float cumulativeSum; 	
};

#endif
