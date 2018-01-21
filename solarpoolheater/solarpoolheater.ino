/********************************************************************/
#include "DataStats.h"
#include "MovingAverage.h"
#include "TemperatureProbes.h"
#include "RealTimeClock.h"

/********************************************************************/

const char SPH_VERSION[] = "0.2";

void setup(void) 
{ 
  // start serial port 
  Serial.begin(9600);
  Serial.print("Version:");
  Serial.println(SPH_VERSION); 
  Serial.println("Setting up"); 
 
  setupTemperatureProbes();
  setupDatalog();
//  setupRTC();

} 

void loop(void) 
{ 
  tickTemperatureProbes();
  processIncomingSerial();
  /********************************************************************/
  tickDatalog();
  
}


