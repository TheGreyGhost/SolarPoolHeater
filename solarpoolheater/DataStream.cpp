#include "DataStream.h"
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "EthernetLink.h"
#include "Datalog.h"
#include "Simulate.h"
#include "TemperatureProbes.h"
#include "SolarIntensity.h"
#include "SystemStatus.h"
#include "PumpControl.h"
#include "Settings.h"
#include "RealTimeClock.h"

const char COMMAND_START_CHAR = '!';

bool sendingLogData;
byte dataRequestID;
long nextEntryToSend;
long numberOfEntriesLeftToSend;

long dataStreamErrorCount = 0;
DataStreamError lastDataStreamError;
int lastDataStreamErrorCode;

// starts a response message with !{command letter echoed}{byte version}
// returns DSE_OK for success, error code otherwise
DataStreamError startResponse(byte commandecho, EthernetUDP * &outConnection)
{
  EthernetUDP *connection;
  bool success = prepareEthernetDatastreamMessage(connection);
  outConnection = connection;
  if (!success || connection == NULL) return DSE_RESPONSE_CONNECTION_FAILED;

  int bytesWritten = 0;
  bytesWritten += connection->write(COMMAND_START_CHAR);
  bytesWritten += connection->write(commandecho);
  bytesWritten += connection->write(DATASTREAM_VERSION_BYTE);

  if (bytesWritten == 3) return DSE_OK;
  return DSE_WRITE_FAILED;
}

// ends a response message with the final byte in the packet which is {byte status: 0 = ok, else error code}
// returns DSE_OK for success, error code otherwise
DataStreamError endResponse(EthernetUDP &connection, byte successCode)
{
  int bytesWritten = connection.write(successCode);
  if (bytesWritten != 1) return DSE_WRITE_FAILED;    

  successCode = connection.endPacket();
  if (0 == successCode) return DSE_ENDPACKET_FAILURE;    

  return DSE_OK;
}

void datastreamLogError(DataStreamError dserror, int errorcode)
{
  lastDataStreamError = dserror;
  lastDataStreamErrorCode = errorcode;
  ++dataStreamErrorCount;
  numberOfEntriesLeftToSend = 0;
}

// perform periodic update of the data stream
void tickDataStream()
{
  if (ethernetStatus != ES_OK) return;
  if (!sendingLogData) return;

  const unsigned long MAX_MILLIS = 250;
  unsigned long startmillis = millis();

  do {
    EthernetUDP *connection; 
    DataStreamError errorcode = startResponse('d', connection);
    if (errorcode != DSE_OK || connection == NULL) {
      datastreamLogError(errorcode, 0);
      return;
    }
    
    int successCode = dataLogPrintEntryBytes(*connection, nextEntryToSend);
    if (successCode != 0) {
      datastreamLogError(DSE_LOGFILE_FAILURE, successCode);
      if (successCode == 7) return; // if write failure to UDP then don't try again.  Otherwise, send error code by UDP
    }
    errorcode = endResponse(*connection, successCode & 0xff);
    if (errorcode != DSE_OK) {
      datastreamLogError(errorcode, 0);
      return;
    }
    
    ++nextEntryToSend;
    --numberOfEntriesLeftToSend;
  
    sendingLogData = (numberOfEntriesLeftToSend > 0);
    if (!sendingLogData) {
      errorcode = startResponse('l', connection);
      if (errorcode == DSE_OK && connection != NULL) {
   	   connection.write(dataRequestID);
  	   errorcode = endResponse(*connection, successCode & 0xff);
      }
      if (connection == NULL || errorcode != DSE_OK) {
        datastreamLogError(errorcode, 0);
        return;
      }
    }		
  } while (sendingLogData && (millis() - startmillis) < MAX_MILLIS);
}

// set up the datastream (EthernetLink already prepared)
void setupDataStream()
{
  sendingLogData = false;
}

// writes the current sensor readings to the given stream
// simulation flags (one bit per simulation variable)
// solarintensity
// cumulative solar intensity (insolation)
// surge tank level switch
// pump runtime (s)
// for each probe: current instant temperature then current smoothed temperature
// last sampled pool temperature: validity, temperature, time (UTC)

DataStreamError sendCurrentSensorReadings(Print &dest)
{
  unsigned int simflags = isBeingSimulatedAll();
  dest.write((byte *)&simflags, sizeof simflags);
  float solarIntensity = NAN;
  if (!solarIntensityReadingInvalid && smoothedSolarIntensity.isValid()) {
    solarIntensity = smoothedSolarIntensity.getEWMA();
  }
  dest.write((byte *)&solarIntensity, sizeof solarIntensity);
  dest.write((byte *)&cumulativeInsolation, sizeof cumulativeInsolation);
  dest.write((byte *)&surgeTankLevelOK, sizeof surgeTankLevelOK);
  dest.write((byte *)&pumpRuntimeSeconds, sizeof pumpRuntimeSeconds);
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    float value = smoothedTemperatures[i].getInstantaneous();
    dest.write((byte *)&value, sizeof value);
    value = smoothedTemperatures[i].getEWMA();
    dest.write((byte *)&value, sizeof value);
  }
  dest.write((byte *)&lastSampledPoolTemperatureIsValid, sizeof lastSampledPoolTemperatureIsValid);
  dest.write((byte *)&lastSampledPoolTemperature, sizeof lastSampledPoolTemperature);
  uint32_t  timestamp = lastSamplePoolTemperatureTime.unixtime();
  dest.write((byte *)&timestamp, sizeof timestamp);

  return DSE_OK;
}

// send all of the current EEPROM settings to the given stream as a byte stream
DataStreamError sendCurrentEEPROMSettings(Print &dest)
{
  EEPROMSettings setting;
  for (setting = SET_FIRST; setting <= SET_INVALID; setting = (EEPROMSettings)((int)setting + 1)) {
    float value = getSetting(setting);
    dest.write((byte *)&value, sizeof value);    
  }
}

/*
!r = request current sensor information (readings)
!s = system status 
!p = request parameter information
!l{byte requestID}{dword row nr}{word count} in LSB first order = request entries from log file
!n = request number of entries in log file
!c = cancel transmissions (log file)
!t{dword unixtime seconds}{long timezone seconds} = set clock time (seconds since unix epoch, in UTC+00:00) and the timezone (in seconds eg +9:30 = 9.5*3600)

response:
!{command letter echoed}{byte version} then:

for sensor readings:
native byte stream of
  uint flags of vbles being simulated
  solar intensity
  cumulative insolation
  surge tank level
  pump runtime
  for each temp probe: instant temp then smoothed temp
invalid values are sent as NAN

for system status
native byte stream of
  assert error
  real time clock status
  log file status
  ethernet status (duh)
  solar sensor status
  temp probe statuses
  pump status
 timezone, resynch status of RTC, synch mismatch (# of seconds that the RTC is ahead of the real time)

for parameter:
native byte stream of all EEPROM settings

for logfile:
!l -> the byte stream from the log file itself, one entry per packet, with 
  command letter !d (for data) followed by
   !l{byte request ID} when finished

!n -> !l{dword number of entries in log file}
!c -> !c{byte request ID}


for time:
{ulong mismatch (+ve = arduino is fast)}{byte RESYNCH_status code}

The final byte in the packet is {byte status: 0 = ok, else error code}
Each response is a single UDP packet only.

*/
// execute the given command
DataStreamError executeDataStreamCommand(const char command[], int commandLength)
{
  bool commandIsValid = false;
  bool sendEndResponse = false;
  DataStreamError errorcode = DSE_OK;
  EthernetUDP *connection;

  if (commandLength >= 3 && command[0] == COMMAND_START_CHAR && command[2] == DATASTREAM_VERSION_BYTE) {
    switch (command[1]) {
      case 's': {  //!s = system status
        commandIsValid = true;

        errorcode = startResponse('s', connection);    
        if (errorcode == DSE_OK && connection != NULL) {
          streamDebugInfo(*connection);
        }
        sendEndResponse = true;        
        break;
      }
      case 'r': {  //!r = request current sensor information (readings)
        commandIsValid = true;

        errorcode = startResponse('r', connection);    
        if (errorcode == DSE_OK && connection != NULL) {
          errorcode = sendCurrentSensorReadings(*connection);
        }
        sendEndResponse = true;        
        break;
      }
      case 'p': {  //!p = request parameter information
        commandIsValid = true;
        errorcode = startResponse('p', connection);    
        if (errorcode == DSE_OK && connection != NULL) {
          errorcode = sendCurrentEEPROMSettings(*connection);
        }
        sendEndResponse = true;        
        break;
      }
      case 'n': {  //!n = request number of entries in log file
        commandIsValid = true;
        unsigned long numberOfSamples = dataLogNumberOfSamples();

        errorcode = startResponse('n', connection);
        if (errorcode == DSE_OK && connection != NULL) {
          int byteswritten = connection->write((byte *)&numberOfSamples, sizeof(numberOfSamples));
          if (byteswritten != sizeof(numberOfSamples)) {
            errorcode = DSE_WRITE_FAILED;
          }
        }
        sendEndResponse = true;        
        break;
      }
      case 'c': {  //!c = cancel transmissions (log file)
        commandIsValid = true;
        sendingLogData = false;
        errorcode = startResponse('c', connection);
        if (errorcode == DSE_OK && connection != NULL) {
          int byteswritten = connection->write(dataRequestID);
          if (byteswritten != sizeof(dataRequestID)) {
            errorcode = DSE_WRITE_FAILED;
          }
        }
        sendEndResponse = true;        
        break;
      }
      case 'l': { //!l{byte request ID}{dword row nr}{word count} in LSB first order = request entries from log file
        if (commandLength < 3 + 1 + 4 + 2) break;
        const byte *bp = (const byte *)(command + 2);
	      dataRequestID = bp[0];
	      bp += 1;
        nextEntryToSend = bp[0] + ((unsigned long)bp[1]<<8) 
                          + ((unsigned long)bp[2]<<16) + ((unsigned long)bp[3]<<24);
        bp += 4;
        numberOfEntriesLeftToSend = bp[0] + ((unsigned long)bp[1]<<8);
        sendingLogData = true;
        commandIsValid = true;
        break;
      }
      case 't': { // !t{dword unixtime seconds}{long timezone seconds} in LSB first order = set clock time (seconds since unix epoch, in UTC0:00) and the timezone (in seconds eg +9:30 = 9.5*3600)
        if (commandLength < 3 + 4 + 4) break;
        const byte *bp = (const byte *)(command + 3);
        unsigned long unixtimeseconds = bp[0] + ((unsigned long)bp[1]<<8) 
                                        + ((unsigned long)bp[2]<<16) + ((unsigned long)bp[3]<<24);
        bp += 4;
        long newtimezone = bp[0] | ((unsigned long)bp[1]<<8) 
                           | ((unsigned long)bp[2]<<16) | ((unsigned long)bp[3]<<24);
        commandIsValid = true;

        long currentClockError = setDateTimeForResynchronisation(unixtimeseconds, newtimezone);
        RESYNCHstatus resynchStatus = getResynchStatus();
        errorcode = startResponse('t', connection);
        if (errorcode == DSE_OK && connection != NULL) {
          int byteswritten = connection->write((byte *)&currentClockError, sizeof(currentClockError));
          byteswritten += connection->write((byte *)&resynchStatus, sizeof(resynchStatus));
          if (byteswritten != sizeof(currentClockError) + sizeof(resynchStatus)) {
            errorcode = DSE_WRITE_FAILED;
          }
        }
        sendEndResponse = true;        

        break;
      }
    }
  }

  if (!commandIsValid) {
    errorcode = DSE_INVALID_COMMAND_RECEIVED;
  } 
  if (errorcode == DSE_OK && sendEndResponse && connection != NULL) {
    errorcode = endResponse(*connection, 0);
  }
  if (errorcode != DSE_OK) {
    datastreamLogError(errorcode, 0);        
  }
  return errorcode;
}
