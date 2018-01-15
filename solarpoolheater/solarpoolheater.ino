/********************************************************************/
// First we include the libraries
#define REQUIRESALARMS false

#include <OneWire.h> 
#include <DallasTemperatureErrorCodes.h>
#include "DataStats.h"
#include "MovingAverage.h"

#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <Wire.h>
#include <RTClib.h>

/********************************************************************/
// define flags
const char SPH_VERSION[] = "0.1";
const bool DEBUG_TEMP = false;

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

const float EWMA_TIME_CONSTANT = 10.0; // decay time in seconds (time to drop to 1/e)
const float EWMA_ALPHA = 0.1;  // corresponds roughly to 10 seconds decay time 
MovingAverage smoothedTemperatures[NUMBER_OF_PROBES] = 
                 {MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT),
                  MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT), MovingAverage(EWMA_ALPHA, EWMA_TIME_CONSTANT)};
                  
DataStats temperatureDataStats[NUMBER_OF_PROBES];
   
enum ProbeStatus probeStatuses[NUMBER_OF_PROBES];
const char* probeNames[NUMBER_OF_PROBES] = {"HX_HOT_INLET", "HX_HOT_OUTLET", "HX_COLD_INLET", "HX_COLD_OUTLET"};

bool echoProbeReadings = false;

const int SD_CHIPSELECT = 4;
File datalogfile;
const int DATALOG_BYTES_PER_SAMPLE = NUMBER_OF_PROBES * 3 * sizeof(float); // each sample has min, avg, max for each probe
const char DATALOG_FILENAME[] = "datalog.txt";

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
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    Serial.println(probeNames[i]);
    Serial.print("  errorCountNotFound:"); Serial.println(errorCountNotFound[i]);
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
  bool commandIsValid = false;
  //Serial.print("Execute command:"); Serial.println(command);  
  switch (command[0]) {
    case 'd': {commandIsValid = true; printDebugInfo(); break;}
    case '?': {
      commandIsValid = true;
      Serial.println("commands (turn CR+LF on):");
      Serial.println("d = print debug info");
      Serial.println("t = toggle echo of probe readings");
      Serial.println("le = erase log file");
      Serial.println("li = log file info");
      Serial.println("lr sample# count = read log data");
      break;
    }
    case 't': {
      commandIsValid = true; 
      echoProbeReadings = !echoProbeReadings;
      break;
    }
    case 'l': {
      switch (command[1]) {
        case 'e': {             // le erase logfile
          commandIsValid = true; 
          bool success;
          datalogfile.close();
          success = SD.remove(DATALOG_FILENAME);
          if (success) {
            datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
            if (datalogfile) {
              Serial.println("data log erased successfully");
            } else {
              success = false;
            }
          }
          if (!success) {
            Serial.println("failed to erase datafile for logged data");
          }
          break;
        }
        case 'i': { //li file info
          commandIsValid = true;
          unsigned long filesize = datalogfile.size();
          Serial.print(filesize / DATALOG_BYTES_PER_SAMPLE);
          Serial.println(" samples");
          break;
        }
        case 'r': {    //lr sample# numberOfSamples - read
          commandIsValid = true;
          char *nextnumber;
          long arg1, arg2;
          arg1 = strtol(command + 2, &nextnumber, 10);
          arg2 = strtol(nextnumber, &nextnumber, 10);
          if (arg1 < 0 || arg2 <= 0) {
            Serial.println("invalid arguments");
          } else {
            unsigned long filesize = datalogfile.size();
            unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;
            if (arg1 >= samplesInFile) {
               Serial.print("arg1 exceeds file size:");
               Serial.println(samplesInFile);
            } else {  
              datalogfile.seek(arg1 * DATALOG_BYTES_PER_SAMPLE);
              long samplesToRead = min(arg2, min(24*60, samplesInFile - arg1));
              for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
                Serial.print(probeNames[i]); 
                Serial.print(" min avg max ");
              }
              Serial.println();
              for (long i = 0; i < samplesToRead; ++i) {
                for (int j = 0; j < NUMBER_OF_PROBES; ++j) {
                  float temp[3];
                  for (int k = 0; k < sizeof(temp); ++k) {
                    ((byte *)temp)[k] = datalogfile.read(); 
                  }
                  for (int k = 0; k < 3; ++k) {
                    Serial.print(temp[k], 1); 
                    Serial.print(" ");
                  }  
                }
                Serial.println();
              }
            }
          }
          break;
        }
      }
      break;
    }
    default: {
      break;
    }
  }

  if (!commandIsValid) {
    Serial.print("unknown command:");
    Serial.println(command);
  }
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

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIPSELECT)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  setupEthernet();

  datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);

  if (!datalogfile) {
    Serial.println("failed to open datafile for writing logged data");
  }

//  setupRTC();

} 

unsigned long temperatureSampleMillis = 0;
bool unreadTemperatures = false;

const unsigned long TEMP_CONVERSION_DELAY = 500; // milliseconds; wait for a suitable length of time (11 bits is 375 ms)
const unsigned long TEMP_SAMPLE_PERIOD = 1000; // milliseconds.  Makes a temperature reading with this period.

void loop(void) 
{ 
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

  processIncomingSerial();
  /********************************************************************/

  if (temperatureDataStats[0].getCount() >= 10) {
//    loopEthernet();
//    loopRTC();
    String dataString = "test";

    // if the file is available, write to it:
    if (datalogfile) {  // don't forget to update DATALOG_BYTES_PER_SAMPLE
      Serial.print("start datafile write:"); Serial.println(millis());
      for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
        float temp[3];
        if (probeStatuses[i] == OK) {
          temp[0] = temperatureDataStats[i].getMin();
          temp[1] = temperatureDataStats[i].getAverage();
          temp[2] = temperatureDataStats[i].getMax();
        } else {
          temp[0] = 0.0; temp[1] = 0.0; temp[2] = 0.0;  
        }
        datalogfile.write((byte *)temp, sizeof temp);
        temperatureDataStats[i].clear();
      }
      Serial.print("start datafile flush:"); Serial.println(millis());
      datalogfile.flush();
      Serial.print("end datafile flush:"); Serial.println(millis());
      Serial.print("file size:");
      Serial.println(datalogfile.size());
    }
  }
}

/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi

 */

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setupEthernet() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loopEthernet() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printDateTime(DateTime dateTime);

RTC_DS1307 RTC;

void setupRTC(void){
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
}

void loopRTC() {
  if (Serial.available() > 0) {
  int instruct = Serial.read();
  switch (instruct) {
    case 'D': {
      DateTime now = RTC.now();
      printDateTime(now);
      break;
    } case 'S':
      RTC.set(RTC_MONTH, 6);
      RTC.set(RTC_HOUR, 16);
      break;
    }
  }
}

void printDateTime(DateTime dateTime) {
  Serial.print(dateTime.year(), DEC);
  Serial.print('/');
  Serial.print(dateTime.month(), DEC);
  Serial.print('/');
  Serial.print(dateTime.day(), DEC);
  Serial.print(' ');
  Serial.print(dateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(dateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(dateTime.second(), DEC);
  Serial.println();
}
