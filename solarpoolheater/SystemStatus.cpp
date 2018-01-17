#include "Debug.h"

int assertFailureCode = 0;

const bool DEBUG_TEMP = false;

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


