/********************************************************************/
#include "SolarPoolHeater.h"
//#include "DataStats.h"
//#include "MovingAverage.h"
//#include "TemperatureProbes.h"
//#include "RealTimeClock.h"
//#include "Datalog.h"
//#include "Commands.h"
//#include "PumpControl.h"
//#include "SolarIntensity.h"
//#include "Simulate.h"

/********************************************************************/

int led = 9;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // start serial port 
  Serial.begin(9600);
  Serial.print("Version:");
  Serial.println(SPH_VERSION); 
  Serial.println("Setting up"); 

//  setupSystemStatus();
//  setupSimulate();
//  setupRTC();
//  setupPumpControl();
//  setupTemperatureProbes();
//  setupDatalog();
//  setupCommands();
//  setupSolarIntensity();
} 


// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

//
//void loop(void) 
//{ 
////  tickRTC();
////  tickTemperatureProbes();
////  tickPumpControl();
////  tickCommands();
////  tickDatalog();
////  tickSystemStatus();
////  tickSolarIntensity();
////  tickSimulate();
//}
