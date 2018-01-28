/********************************************************************/
#include "SolarPoolHeater.h"
#include "DataStats.h"
#include "MovingAverage.h"
#include "TemperatureProbes.h"
#include "RealTimeClock.h"
#include "Datalog.h"
#include "Commands.h"

/********************************************************************/

void setup(void) 
{ 
  // start serial port 
  Serial.begin(9600);
  Serial.print("Version:");
  Serial.println(SPH_VERSION); 
  Serial.println("Setting up"); 

  setupSystemStatus();
  setupTemperatureProbes();
  setupDatalog();
  setupCommands();

//  setupRTC();

} 

void loop(void) 
{ 
  tickTemperatureProbes();
  tickCommands();
  tickDatalog();
  tickSystemStatus();
}


