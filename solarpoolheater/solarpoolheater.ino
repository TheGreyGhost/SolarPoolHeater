/********************************************************************/
// First we include the libraries
#define REQUIRESALARMS false

#include <OneWire.h> 
#include <DallasTemperatureErrorCodes.h>
/********************************************************************/
// define flags
const char SPH_VERSION[] = "0.1";
const bool DEBUG_TEMP = true;

int assertFailureCode = 0;
#define ASSERT_INVALID_SWITCH 1
#define ASSERT_INDEX_OUT_OF_BOUNDS 2

/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
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

void printDebugInfo()
{
  Serial.print("Version:"); Serial.println(SPH_VERSION); 
  Serial.print("Last Assert Error:"); Serial.println(assertFailureCode); 
  Serial.print("errorCountBusFailure:"); Serial.println(errorCountBusFailure);
  for (int i = 0; i < NUMBEROFPROBES; ++i) {
    Serial.println(probeNames[i]);
    Serial.print("  errorCountNotFound:"); Serial.println(errorCoutNotFound[i]);
    Serial.print("  errorCountCRCFailure:"); Serial.println(errorCountCRCFailure[i]);
    Serial.print("  errorCountImplausibleValue:"); Serial.println(errorCountImplausibleValue[i]);
    Serial.print("    errorLastImplausibleValue:"); Serial.print(errorLastImplausibleValueRaw[i]);
      Serial.print(" "); Serial.println(errorLastImplausibleValueC[i]);
  }
}

const int MAX_COMMAND_LENGTH = 30;
const int COMMAND_BUFFER_SIZE = MAX_COMMAND_LENGTH + 2;  // if buffer fills to max size, truncation occurs
int commandBufferIdx = 0;
char commandBuffer[COMMAND_BUFFER_SIZE];  
const char COMMAND_START_CHAR = '!';

// execute the command encoded in commandString.  Null-terminated
void executeCommand(char command[]) 
{
  Serial.print("Execute command:"); Serial.println(command);  
}

// look for incoming serial input (commands); collect the command and execute it when the entire command has arrived.
void processIncomingSerial()
{
  while (Serial.available()) {
    if (commandBufferIdx < 0  || commandBufferIdx > COMMAND_BUFFER_SIZE) {
      assertFailureCode = ASSERT_INDEX_OUT_OF_BOUNDS;
      commandBufferIdx = 0;
    }
    int nextChar = Serial.read();
    if (nextChar == COMMAND_START_CHAR) {
      commandBufferIdx = 0;        
    } else if (nextChar == '\n') {
      if (commandBufferIdx > 0) {
        if (commandBufferIdx > MAX_COMMAND_LENGTH) {
          commandBuffer[MAX_COMMAND_LENGTH] = '\0';
          Serial.print("Command too long:"); Serial.println(commandBuffer);
        } else {
          commandBuffer[commandBufferIdx++] = '\0';
          executeCommand(commandBuffer);
        } 
        commandBufferIdx = 0; 
      }
    } else {
      if (commandBufferIdx >= 0 && commandBufferIdx < COMMAND_BUFFER_SIZE) {
        commandBuffer[commandBufferIdx++] = nextChar;
      }
    }
  }
}

void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600);
 Serial.print("Version:");
 Serial.println(SPH_VERSION); 
 Serial.println("Setting up"); 
 // Start up the library 
 sensors.begin(); 
 bool success;
 success = enumerateProbes();
 Serial.println(success ? "OK" : "ERROR");
 sensors.setResolution(TEMP_11_BIT); //11 bit is 0.125 C
 sensors.setWaitForConversion(false);
} 

unsigned long temperatureSampleMillis = 0;

const unsigned long TEMP_CONVERSION_DELAY = 1000; // milliseconds; wait for a suitable length of time (11 bits is 375 ms)

void loop(void) 
{ 
  unsigned long timeNow = millis();
  if (timeNow - temperatureSampleMillis > TEMP_CONVERSION_DELAY) { // read values from sensors
    int i;
    for (i = 0; i < NUMBER_OF_PROBES; ++i) {
      int16_t tempValue = sensors.getTemp(probeAddresses[i]);
      float tempValueCelcius = sensors.rawToCelsius(tempValue);
      if (tempValue == DEVICE_DISCONNECTED_RAW) {
        switch(sensors.getLastErrorCode()) {
          case CRC_FAIL: probeStatuses[i] = CRC_FAILURE; ++errorCountCRCFailure[i]; break;
          case RESET_FAIL: probeStatuses[i] = BUS_FAILURE; ++errorCountBusFailure; break;
          case DEVICE_NOT_FOUND: probeStatuses[i] = NOT_FOUND; ++errorCountNotFound; break;
          default: assertFailureCode = ASSERT_INVALID_SWITCH; probeStatuses[i] = NOT_FOUND; ++errorCountNotFound; break; 
        }
      } else {
        if (tempValueCelcius < MIN_PLAUSIBLE_TEMPERATURE || tempValueCelcius > MAX_PLAUSIBLE_TEMPERATURE) {
          probeStatuses[i] = IMPLAUSIBLE_VALUE;
          ++errorCountImplausibleValue[i]; 
          errorCountLastInfeasibleValueRaw[i] = tempValue;
          errorCountLastInfeasibleValueC[i] = tempValueCelcius;
        } else {
          probeStatuses[i] = OK;
        }
      }
   
      if (DEBUG_TEMP) {
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
      }  // DEBUG_TEMP
    }  // for i = 0 to NUMBER_OF_PROBES
  } else if (timeNow - temperatureSampleMillis > TEMP_CONVERSION_DELAY) { // start next conversion
    if (DEBUG_TEMP) {
      Serial.print("temp start at "); 
      Serial.println(timeNow);
    }
    bool success = sensors.requestTemperatures(); // Send the command to get temperature readings 
    temperatureSampleMillis = timeNow;
  }

  processIncomingSerial();
  /********************************************************************/
  delay(1000); 
}

