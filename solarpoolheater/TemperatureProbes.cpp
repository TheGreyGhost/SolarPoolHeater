#include "TemperatureProbes.h"
#include <OneWire.h> 

#define REQUIRESALARMS false // Don't need the temp probe alarms
#include <DallasTemperatureErrorCodes.h>

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
const int HX_HOT_INLET = 0;
const int HX_HOT_OUTLET = 1;
const int HX_COLD_INLET = 2;
const int HX_COLD_OUTLET = 3;
const int LAST_PROBE_IDX = HX_COLD_OUTLET;
const int NUMBER_OF_PROBES = LAST_PROBE_IDX + 1;
const int BYTESPERADDRESS = 8;

enum ProbeStatus {OK, NOT_FOUND, BUS_FAILURE, CRC_FAILURE, IMPLAUSIBLE_VALUE};

unsigned long errorCountBusFailure = 0;
unsigned long errorCountNotFound[NUMBER_OF_PROBES];
unsigned long errorCountCRCFailure[NUMBER_OF_PROBES];
unsigned long errorCountImplausibleValue[NUMBER_OF_PROBES];
int16_t errorLastImplausibleValueRaw[NUMBER_OF_PROBES];
float errorLastImplausibleValueC[NUMBER_OF_PROBES];

// temperatures outside this range will be considered infeasible
const float MIN_PLAUSIBLE_TEMPERATURE = -5.0;
const float MAX_PLAUSIBLE_TEMPERATURE = 120.0;

DeviceAddress probeAddresses[NUMBER_OF_PROBES] = 
   {{0x28, 0xFF, 0x19, 0xE6, 0x81, 0x17, 0x04, 0xAD},  // HX_HOT_INLET
    {0x28, 0xFF, 0x43, 0x03, 0x81, 0x17, 0x05, 0x13},  // HX_HOT_OUTLET
    {0x28, 0xFF, 0x83, 0x04, 0x81, 0x17, 0x05, 0xB7},  // HX_COLD_INLET
    {0x28, 0xFF, 0x90, 0x1A, 0x81, 0x17, 0x05, 0xA8}   // HX_COLD_OUTLET
   };

const uint8_t *HX_HOT_INLET_ADDRESS = probeAddresses[HX_HOT_INLET];
const uint8_t *HX_HOT_OUTLET_ADDRESS = probeAddresses[HX_HOT_OUTLET];
const uint8_t *HX_COLD_INLET_ADDRESS = probeAddresses[HX_COLD_INLET];
const uint8_t *HX_COLD_OUTLET_ADDRESS = probeAddresses[HX_COLD_OUTLET];

const float EWMA_TIME_CONSTANT = 10.0; // decay time in seconds (time to drop to 1/e) assuming 1 Hz sampling frequency
const float EWMA_ALPHA = 0.1;  // corresponds roughly to 10 seconds decay time assuming 1 Hz sampling frequency
MovingAverage smoothedTemperatures[NUMBER_OF_PROBES] = 
                 {MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT),
                  MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT)};
                  
DataStats temperatureDataStats[NUMBER_OF_PROBES];
   
enum ProbeStatus probeStatuses[NUMBER_OF_PROBES];
const char* probeNames[NUMBER_OF_PROBES] = {"HX_HOT_INLET", "HX_HOT_OUTLET", "HX_COLD_INLET", "HX_COLD_OUTLET"};

bool echoProbeReadings = false;

void copyProbeAddress(DeviceAddress dest, DeviceAddress source) {
  int i;
  for (i = 0; i < BYTESPERADDRESS; ++i) {
    dest[i] = source[i];
  }
}

// prints a byte as two digit hex
void printByteHex(uint8_t value) {
  Serial.print(value >> 4, HEX);
  Serial.print(value & 15, HEX);
}

// prints a device address as hex bytes eg "0x35, 0xF3, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08"
void printAddress(DeviceAddress address) {
  int j;
  for (j = 0; j < 8; ++j) {
    Serial.print("0x");       
    printByteHex(address[j]);
    if (j != 7) {
      Serial.print(", ");
    }
  }  
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

// compares the two device addresses - returns true if match, false otherwise
bool doesAddressMatch(DeviceAddress addr1, DeviceAddress addr2) {
  int i;
  for (i = 0; i < BYTESPERADDRESS; ++i) {
    if (addr1[i] != addr2[i]) return false;
  }
  return true;
}

// find all sensors on the bus, match them to the expected sensor addresses
// returns true for success, or false if something unexpected occurred
bool enumerateProbes() {
  int numberOfSensorsFound = sensors.getDeviceCount();
  Serial.print("Number of sensors found:");
  Serial.println(numberOfSensorsFound);

  DeviceAddress addressesFound[numberOfSensorsFound];
  int i;
  bool found;
  numberOfSensors = 0;
  for (i = 0; i < numberOfSensorsFound; ++i) {
    found = sensors.getAddress(addressesFound[i], i);
    Serial.print("ID: ");
    if (found) {
      printAddress(addressesFound[i]); 
    } else {
      Serial.print("error"); 
    }
    Serial.println();
  }

  bool failedToFind = false;
  for (i = 0; i < NUMBER_OF_PROBES; ++i) {
    Serial.print(probeNames[i]);
    Serial.print(" expects ");
    printAddress(probeAddresses[i]); 
    if (doesAddressMatchAny(probeAddresses[i], addressesFound, numberOfSensorsFound)) {
      Serial.println(" (found) ");
      probeStatuses[i] = OK;
    } else {
      Serial.println(" (not found) ");
      probeStatuses[i] = NOT_FOUND;
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
 Serial.println(success ? "OK" : "ERROR");
 sensors.setResolution(TEMP_11_BIT); //11 bit is 0.125 C
 sensors.setWaitForConversion(false);


}

void tickTemperatureProbes()
{
  // there is a bug here; the first logged sample for each probe says max is 85 C - is that the default of the sensor?
  
    unsigned long timeNow = millis();
  if (unreadTemperatures && timeNow - temperatureSampleMillis > TEMP_CONVERSION_DELAY) { // read values from sensors
    int i;
    for (i = 0; i < NUMBER_OF_PROBES; ++i) {
      int16_t tempValue = sensors.getTemp(probeAddresses[i]);
      float tempValueCelcius = sensors.rawToCelsius(tempValue);
      if (tempValue == DEVICE_DISCONNECTED_RAW) {
        switch(sensors.getLastError()) {
          case CRC_FAIL: probeStatuses[i] = CRC_FAILURE; ++errorCountCRCFailure[i]; break;
          case RESET_FAIL: probeStatuses[i] = BUS_FAILURE; ++errorCountBusFailure; break;
          case DEVICE_NOT_FOUND: probeStatuses[i] = NOT_FOUND; ++errorCountNotFound[i]; break;
          default: assertFailureCode = ASSERT_INVALID_SWITCH; probeStatuses[i] = NOT_FOUND; ++errorCountNotFound[i]; break; 
        }
      } else {
        if (tempValueCelcius < MIN_PLAUSIBLE_TEMPERATURE || tempValueCelcius > MAX_PLAUSIBLE_TEMPERATURE) {
          probeStatuses[i] = IMPLAUSIBLE_VALUE;
          ++errorCountImplausibleValue[i]; 
          errorLastImplausibleValueRaw[i] = tempValue;
          errorLastImplausibleValueC[i] = tempValueCelcius;
        } else {
          probeStatuses[i] = OK;
          temperatureDataStats[i].addDatapoint(tempValueCelcius);
          smoothedTemperatures[i].addDatapoint(tempValueCelcius);
        }
      }

      if (echoProbeReadings) {
        Serial.print(probeNames[i]);
        Serial.print(":");
        if (probeStatuses[i] == OK) {
          Serial.println(sensors.rawToCelsius(tempValue));
        } else {
          switch (probeStatuses[i]) {
            case NOT_FOUND: Serial.println("Not found"); break;
            case BUS_FAILURE: Serial.println("Bus failure"); break;
            case CRC_FAILURE: Serial.println("CRC failure"); break;
            case IMPLAUSIBLE_VALUE: Serial.println("Invalid status"); break;
            default: Serial.println("Invalid status"); break;
          }
        }
      }  // echoProbeReadings
    }  // for i = 0 to NUMBER_OF_PROBES
    unreadTemperatures = false;
  } else if (timeNow - temperatureSampleMillis >= TEMP_SAMPLE_PERIOD) { // start next conversion
    if (DEBUG_TEMP) {
      Serial.print("temp start at "); 
      Serial.println(timeNow);
    }
    bool success = sensors.requestTemperatures(); // Send the command to get temperature readings 
    temperatureSampleMillis = timeNow;
    unreadTemperatures = true;
  }

}


