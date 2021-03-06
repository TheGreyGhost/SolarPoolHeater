/********************************************************************/
#include "SolarPoolHeater.h"
#include "DataStats.h"
#include "MovingAverage.h"
#include "TemperatureProbes.h"
#include "RealTimeClock.h"
#include "Datalog.h"
#include "Commands.h"
#include "PumpControl.h"
#include "SolarIntensity.h"
#include "Simulate.h"

/********************************************************************/

void setup(void) 
{ 
  // start serial port 
  Serial.begin(9600);
  Serial.print("Version:");
  Serial.println(SPH_VERSION); 
  Serial.println("Setting up"); 

  setupSystemStatus();
  setupSimulate();
  setupRTC();
  setupPumpControl();
  setupTemperatureProbes();
  setupDatalog();
  setupCommands();
  setupSolarIntensity();
} 

void loop(void) 
{ 
  tickRTC();
  tickTemperatureProbes();
  tickPumpControl();
  tickCommands();
  tickDatalog();
  tickSystemStatus();
  tickSolarIntensity();
  tickSimulate();
}


