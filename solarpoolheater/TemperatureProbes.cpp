#include "TemperatureProbes.h"
#include <OneWire.h> 

#define REQUIRESALARMS false // Don't need the temp probe alarms
#include <DallasTemperatureErrorCodes.h>
#include "Simulate.h"

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define TEMP_PROBES_ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(TEMP_PROBES_ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperatureErrorCodes sensors(&oneWire);
/********************************************************************/ 

uint8_t numberOfSensors;
const int BYTESPERADDRESS = 8;

unsigned long errorCountBusFailure = 0;
unsigned long errorCountNotFound[NUMBER_OF_PROBES];
unsigned long errorCountCRCFailure[NUMBER_OF_PROBES];
unsigned long errorCountImplausibleValue[NUMBER_OF_PROBES];
int16_t errorLastImplausibleValueRaw[NUMBER_OF_PROBES];
float errorLastImplausibleValueC[NUMBER_OF_PROBES];

// temperatures outside this range will be considered infeasible
const float MIN_PLAUSIBLE_TEMPERATURE = -5.0;
const float MAX_PLAUSIBLE_TEMPERATURE = 120.0;

const int16_t INITIAL_VALUE_RAW = 0x0550;  // this is the default powerup value; 85.0 C.  If we ever see it, just ignore it.

DeviceAddress probeAddresses[NUMBER_OF_PROBES] = 
   {{0x28, 0xFF, 0xCC, 0x47, 0x71, 0x17, 0x03, 0x09},  // HX_HOT_INLET
    {0x28, 0xFF, 0xE4, 0x9B, 0x81, 0x17, 0x04, 0xD2},  // HX_HOT_OUTLET
    {0x28, 0xFF, 0x90, 0x1A, 0x81, 0x17, 0x05, 0xA8},  // HX_COLD_INLET
    {0x28, 0xFF, 0x83, 0x04, 0x81, 0x17, 0x05, 0xB7},  // HX_COLD_OUTLET
    {0x28, 0xFF, 0x6B, 0x94, 0x81, 0x17, 0x04, 0xEC}   // HX_AMBIENT
   };

const uint8_t *HX_HOT_INLET_ADDRESS = probeAddresses[HX_HOT_INLET];
const uint8_t *HX_HOT_OUTLET_ADDRESS = probeAddresses[HX_HOT_OUTLET];
const uint8_t *HX_COLD_INLET_ADDRESS = probeAddresses[HX_COLD_INLET];
const uint8_t *HX_COLD_OUTLET_ADDRESS = probeAddresses[HX_COLD_OUTLET];
const uint8_t *AMBIENT_ADDRESS = probeAddresses[AMBIENT];

const float EWMA_TIME_CONSTANT = 10.0; // decay time in seconds (time to drop to 1/e) assuming 1 Hz sampling frequency
const float EWMA_ALPHA = 0.1;  // corresponds roughly to 10 seconds decay time assuming 1 Hz sampling frequency
MovingAverage smoothedTemperatures[NUMBER_OF_PROBES] = 
                 {MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT),
                  MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT),
                  MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT)};
                  
DataStats temperatureDataStats[NUMBER_OF_PROBES];
   
volatile enum ProbeStatus probeStatuses[NUMBER_OF_PROBES];
const char* probeNames[NUMBER_OF_PROBES] = {"HX_HOT_INLET", "HX_HOT_OUTLET", "HX_COLD_INLET", "HX_COLD_OUTLET", "AMBIENT"};

bool echoProbeReadings = false;

void copyProbeAddress(DeviceAddress dest, DeviceAddress source) {
  int i;
  for (i = 0; i < BYTESPERADDRESS; ++i) {
    dest[i] = source[i];
  }
}

// prints a byte as two digit hex
void printByteHex(uint8_t value) {
  console->print(value >> 4, HEX);
  console->print(value & 15, HEX);
}

// prints a device address as hex bytes eg "0x35, 0xF3, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08"
void printAddress(DeviceAddress address) {
  int j;
  for (j = 0; j < 8; ++j) {
    console->print("0x");       
    printByteHex(address[j]);
    if (j != 7) {
      console->print(", ");
    }
  }  
}

// compares the two device addresses - returns true if match, false otherwise
bool doesAddressMatch(DeviceAddress addr1, DeviceAddress addr2) {
  int i;
  for (i = 0; i < BYTESPERADDRESS; ++i) {
    if (addr1[i] != addr2[i]) return false;
  }
  return true;
}

// checks whether DeviceAddress addrToFind matches any of the addresses in addressesToSearch
// returns true if found, false otherwise.
// numOfAddresses is the number of addresses in addressesToSearch
bool doesAddressMatchAny(DeviceAddress addrToFind, DeviceAddress addressesToSearch[], int numOfAddresses) {
  int i;
  for (i = 0; i < numOfAddresses; ++i) {
    if (doesAddressMatch(addrToFind, addressesToSearch[i])) {
      return true;
    }
  }
  return false;
}

// find all sensors on the bus, match them to the expected sensor addresses
// returns true for success, or false if something unexpected occurred
bool enumerateProbes() {
  int numberOfSensorsFound = sensors.getDeviceCount();
  console->print("Number of sensors found:");
  console->println(numberOfSensorsFound);

  DeviceAddress addressesFound[numberOfSensorsFound];
  int i;
  bool found;
  numberOfSensors = 0;
  for (i = 0; i < numberOfSensorsFound; ++i) {
    found = sensors.getAddress(addressesFound[i], i);
    console->print("ID: ");!s
    if (found) {
      printAddress(addressesFound[i]); 
    } else {
      console->print("error"); 
    }
    console->println();
  }

  bool failedToFind = false;
  for (i = 0; i < NUMBER_OF_PROBES; ++i) {
    console->print(probeNames[i]);
    console->print(" expects ");
    printAddress(probeAddresses[i]); 
    if (doesAddressMatchAny(probeAddresses[i], addressesFound, numberOfSensorsFound)) {
      console->println(" (found) ");
      probeStatuses[i] = PS_OK;
    } else {
      console->println(" (not found) ");
      probeStatuses[i] = PS_NOT_FOUND;
      failedToFind = true;
    }
  }
  return !failedToFind;
}


unsigned long temperatureSampleMillis = 0;  // time at which the last temperature sampling was initiated
bool unreadTemperatures = false;            // has the temperature sampling been initiated but the results not yet read? 

const unsigned long TEMP_CONVERSION_DELAY = 500; // milliseconds; wait for a suitable length of time (11 bits is 375 ms)
const unsigned long TEMP_SAMPLE_PERIOD = 1000; // milliseconds.  Makes a temperature reading with this period.

void setupTemperatureProbes()
{
   // Start up the library 
 sensors.begin(); 
 bool success;
 success = enumerateProbes();
 console->println(success ? "All Temperature Probes OK" : "Temperature Probe ERROR");
 sensors.setResolution(TEMP_11_BIT); //11 bit is 0.125 C
 sensors.setWaitForConversion(false);
}

void tickTemperatureProbes()
{
  unsigned long timeNow = millis();
  if (unreadTemperatures && timeNow - temperatureSampleMillis > TEMP_CONVERSION_DELAY) { // read values from sensors
    int i;
    for (i = 0; i < NUMBER_OF_PROBES; ++i) {
      int16_t tempValue = sensors.getTemp(probeAddresses[i]);
      float tempValueCelcius = sensors.rawToCelsius(tempValue);
      tempValueCelcius = getSimulatedValue((SimVariables)i, tempValueCelcius);
      
      if (!isBeingSimulated((SimVariables)i) && tempValue == DEVICE_DISCONNECTED_RAW) {
        switch(sensors.getLastError()) {
          case CRC_FAIL: probeStatuses[i] = PS_CRC_FAILURE; ++errorCountCRCFailure[i]; break;
          case RESET_FAIL: probeStatuses[i] = PS_BUS_FAILURE; ++errorCountBusFailure; break;
          case DEVICE_NOT_FOUND: probeStatuses[i] = PS_NOT_FOUND; ++errorCountNotFound[i]; break;
          default: assertFailureCode = ASSERT_INVALID_SWITCH; probeStatuses[i] = PS_NOT_FOUND; ++errorCountNotFound[i]; break; 
        }
      } else if (tempValue == INITIAL_VALUE_RAW) {
          probeStatuses[i] = PS_OK;
          // don't store anything
      } else  {
        if (tempValueCelcius < MIN_PLAUSIBLE_TEMPERATURE || tempValueCelcius > MAX_PLAUSIBLE_TEMPERATURE) {
          probeStatuses[i] = PS_IMPLAUSIBLE_VALUE;
          ++errorCountImplausibleValue[i]; 
          errorLastImplausibleValueRaw[i] = tempValue;
          errorLastImplausibleValueC[i] = tempValueCelcius;
        } else {
          probeStatuses[i] = PS_OK;
          temperatureDataStats[i].addDatapoint(tempValueCelcius);
          smoothedTemperatures[i].addDatapoint(tempValueCelcius);
        }
      }

      if (echoProbeReadings) {
        if (probeStatuses[i] == PS_OK) {
          console->print(tempValueCelcius);
        } else {
          switch (probeStatuses[i]) {
            case PS_NOT_FOUND: console->print("Not found"); break;
            case PS_BUS_FAILURE: console->print("Bus failure"); break;
            case PS_CRC_FAILURE: console->print("CRC failure"); break;
            case PS_IMPLAUSIBLE_VALUE: console->print("Invalid status"); break;
            default: console->print("Invalid status"); break;
          }
        }
        console->print(" : ");
      }  // echoProbeReadings
    }  // for i = 0 to NUMBER_OF_PROBES
    if (echoProbeReadings) {
      console->println();
    }  
    unreadTemperatures = false;
  } else if (timeNow - temperatureSampleMillis >= TEMP_SAMPLE_PERIOD) { // start next conversion
    if (DEBUG_TEMP) {
      console->print("temp start at "); 
      console->println(timeNow);
    }
    bool success = sensors.requestTemperatures(); // Send the command to get temperature readings 
    temperatureSampleMillis = timeNow;
    unreadTemperatures = true;
  }

}


