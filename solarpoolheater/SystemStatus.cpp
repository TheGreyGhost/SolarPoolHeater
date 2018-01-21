#include "Debug.h"

int assertFailureCode = 0;

const bool DEBUG_TEMP = false;

const byte ERRORCODE_PROBE = 16;   // leave space for NUMBER_OF_PROBES, ie 16 = probe 0, 17 = probe 1, etc
const byte ERRORCODE_DATALOG = 32; // leave space for error codes

void printDebugInfo()
{
  Serial.print("Version:"); Serial.println(SPH_VERSION); 
  Serial.print("Last Assert Error:"); Serial.println(assertFailureCode); 
  Serial.print("errorCountBusFailure:"); Serial.println(errorCountBusFailure);
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    Serial.println(probeNames[i]);
    Serial.print("  errorCountNotFound:"); Serial.println(errorCountNotFound[i]);
    Serial.print("  errorCountCRCFailure:"); Serial.println(errorCountCRCFailure[i]);
    Serial.print("  errorCountImplausibleValue:"); Serial.println(errorCountImplausibleValue[i]);
    Serial.print("    errorLastImplausibleValue:"); Serial.print(errorLastImplausibleValueRaw[i]);
      Serial.print(" "); Serial.println(errorLastImplausibleValueC[i]);
  }
}

const int MAX_ERROR_DEPTH = 16;
bool errorLEDstate;
long lastErrorLEDtime;
byte errorStack[MAX_ERROR_DEPTH];
byte errorStackIdx;
byte currentErrorBitpos;
byte currentErrorForLED;

void setupSystemStatus()
{

  
}


void tickSystemStatus()
{
  
}

void populateErrorStack()
{
  errorStackIdx = 0;
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    if (probeStatuses[i] <> OK) {
      errorStack[errorStackIdx++] = ERRORCODE_PROBE | i;
    }
  }
  if (logfileStatus != OK) {
    errorStack[errorStackIdx++] = ERRORCODE_DATALOG | logfileStatus;
  }
}

const int 

void tickLEDstate()
{
  

  
}



