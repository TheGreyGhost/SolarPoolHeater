#ifndef SIMULATE_H
#define SIMULATE_H

// these values must match TemperatureProbes:: probe numbers
enum SimVariables {SIM_FIRST = 0, SIM_HX_HOT_INLET = 0, SIM_HX_HOT_OUTLET = 1, SIM_HX_COLD_INLET = 2, SIM_HX_COLD_OUTLET = 3, SIM_AMBIENT = 4,
                   SIM_SOLAR_INTENSITY = 5, SIM_SURGE_TANK_LEVEL = 6, SIM_LAST_PLUS_ONE = 7};
const int NUMBER_OF_SIM_VARIABLES = (int)SIM_LAST_PLUS_ONE;

// for all functions: if whichVariable is out of range, no effect / acts as if unsimulated

// gets the simulated value for the desired variable
// returns: the simulated value, or the actual value if this vble isn't being simulated
// if whichVariable is out of range, returns actualValue
float getSimulatedValue(SimVariables whichVariable, float actualValue);

// changes the given setting to its default value
// returns: the new value.
// if whichVariable is out of range, does nothing
void setSimulatedValue(SimVariables whichVariable, float simulationValue);

// returns true if this variable is being simulated
bool isBeingSimulated(SimVariables whichVariable);

// stop simulating for the indicated variable(s)
// if whichVariable is out of range, does nothing
void stopSimulating(SimVariables whichVariable);
void stopSimulatingAll();

const char *getSimulationLabel(SimVariables whichVariable);

void setupSimulate();

void tickSimulate();

// return a flag showing which vbles are being simulated (each bit = 2<<SimVariables)
int isBeingSimulatedAll();

#endif
