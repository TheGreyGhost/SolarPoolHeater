#include "Simulate.h"

bool simulationActive[NUMBER_OF_SIM_VARIABLES];
float simulationValues[NUMBER_OF_SIM_VARIABLES];

// gets the simulated value for the desired variable
// returns: the simulated value, or the actual value if this vble isn't being simulated
float getSimulatedValue(SimVariables whichVariable, float actualValue)
{
  if (whichVariable < SIM_FIRST || whichVariable >= SIM_LAST_PLUS_ONE) return actualValue;
  return simulationActive[whichVariable] ? simulationValues[whichVariable] : actualValue;
}

bool isBeingSimulated(SimVariables whichVariable)
{
  if (whichVariable < SIM_FIRST || whichVariable >= SIM_LAST_PLUS_ONE) return false;
  return simulationActive[whichVariable];
}

// return a flag showing which vbles are being simulated (each bit = 2<<SimVariables)
unsigned int isBeingSimulatedAll()
{
  unsigned int retval = 0;
  for (SimVariables i = SIM_LAST_PLUS_ONE; i > SIM_FIRST ; --i) {
    retval = (retval << 1) | (simulationActive[i - 1] ? 1 : 0);
  }
  return retval;
}

// changes the given setting to its default value
// returns: the new value.
void setSimulatedValue(SimVariables whichVariable, float simulationValue)
{
  if (whichVariable < SIM_FIRST || whichVariable >= SIM_LAST_PLUS_ONE) return;
  simulationActive[whichVariable] = true;
  simulationValues[whichVariable] = simulationValue;
}

// stop simulating for the indicated variable(s)
void stopSimulating(SimVariables whichVariable)
{
  if (whichVariable < SIM_FIRST || whichVariable >= SIM_LAST_PLUS_ONE) return;
  simulationActive[whichVariable] = false;
}

void stopSimulatingAll()
{
  SimVariables i;
  for (i = SIM_FIRST; i < SIM_LAST_PLUS_ONE; i = (SimVariables)((int)i + 1)) {
    simulationActive[i] = false;
  }
}

const char* simulationLabels[NUMBER_OF_SIM_VARIABLES] = {"HX_HOT_INLET", "HX_HOT_OUTLET", "HX_COLD_INLET", "HX_COLD_OUTLET", "AMBIENT", "SOLAR_INTENSITY", "SURGE_TANK_LEVEL_OK"};

const char *getSimulationLabel(SimVariables whichVariable)
{
  if (whichVariable < SIM_FIRST || whichVariable >= SIM_LAST_PLUS_ONE) return "INVALID";
  return simulationLabels[(int)whichVariable];
}

void setupSimulate()
{
  // simulationActive default to all off
}

void tickSimulate()
{
  
}

