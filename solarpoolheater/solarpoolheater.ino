/********************************************************************/
// First we include the libraries
#define REQUIRESALARMS false

#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

uint8_t numberOfSensors;

const int HX_HOT_INLET = 0;
const int HX_HOT_OUTLET = 1;
const int HX_COLD_INLET = 2;
const int HX_COLD_OUTLET = 3;
const int LAST_PROBE_IDX = HX_COLD_OUTLET;
const int NUMBER_OF_PROBES = LAST_PROBE_IDX + 1;
const int BYTESPERADDRESS = 8;

enum ProbeStatus {OK, NOT_FOUND, DISCONNECTED, CRC_FAIL, INFEASIBLE_VALUE};

DeviceAddress probeAddresses[NUMBER_OF_PROBES] = 
   {{0x28, 0xFF, 0x19, 0xE6, 0x81, 0x17, 0x04, 0xAD},  // HX_HOT_INLET
    {0x28, 0xFF, 0x43, 0x03, 0x81, 0x17, 0x05, 0x13},  // HX_HOT_OUTLET
    {0x28, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // HX_COLD_INLET
    {0x28, 0xFF, 0x43, 0x03, 0x81, 0x00, 0x00, 0x00}   // HX_COLD_OUTLET
   };

const uint8_t *HX_HOT_INLET_ADDRESS = probeAddresses[HX_HOT_INLET];
const uint8_t *HX_HOT_OUTLET_ADDRESS = probeAddresses[HX_HOT_OUTLET];
const uint8_t *HX_COLD_INLET_ADDRESS = probeAddresses[HX_COLD_INLET];
const uint8_t *HX_COLD_OUTLET_ADDRESS = probeAddresses[HX_COLD_OUTLET];
   
enum ProbeStatus probeStatuses[NUMBER_OF_PROBES];
const char* probeNames[] = {"HX_HOT_INLET", "HX_HOT_OUTLET", "HX_COLD_INLET", "HX_COLD_OUTLET"};

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

void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Setting up"); 
 // Start up the library 
 sensors.begin(); 
 bool success;
 success = enumerateProbes();
 Serial.println(success ? "OK" : "ERROR");
} 

void loop(void) 
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus 
  /********************************************************************/
  Serial.print("start-"); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  Serial.println("done"); 
  /********************************************************************/
  int i;
  for (i = 0; i < NUMBER_OF_PROBES; ++i) {
    Serial.print(probeNames[i]);
    Serial.print(":");
    int16_t tempValue = sensors.getTemp(probeAddresses[i]);
    if (tempValue == DEVICE_DISCONNECTED_RAW) {
      Serial.println("DISCONNECTED");
    } else {
      Serial.println(sensors.rawToCelsius(tempValue));
    }
  }
  delay(1000); 
}

