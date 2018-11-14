#include "DataStream.h"

const char COMMAND_START_CHAR = '!';

bool sendingLogData;
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
  bool success = prepareEthernetDatastreamMessage(&connection);
  outConnection = connection;
  if (!success) return DSE_RESPONSE_CONNECTION_FAILED;

  int bytesWritten = 0;
  bytesWritten += connection.write(COMMAND_START_CHAR);
  bytesWritten += connection.write(commandecho);
  bytesWritten += connection.write(DATASTREAM_VERSION_BYTE);

  if (bytesWritten == 3) return DSE_OK;
  return DSE_WRITE_FAILED;
}

// ends a response message with the final byte in the packet which is {byte status: 0 = ok, else error code}
// returns DSE_OK for success, error code otherwise
DataStreamError endResponse(EthernetUDP * connection, byte successcode)
{
  int bytesWritten = connection->write(successCode);
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
  numberofEntriesLeftToSend = 0;
}

// perform periodic update of the data stream
void tickDataStream()
{
  if (ethernetStatus != ES_OK) return;
  if (!sendingLogData) return;

  EthernetUDP *connection; 
  DataStreamError errorcode = startResponse('l', &connection);
  if (errorcode != DSE_OK)) {
    datastreamLogError(errorcode, 0);
    return;
  }
  
  int successCode = dataLogSendEntryBytes(connection, nextEntryToSend);
  if (successCode != 0) {
    datastreamLogError(DSE_LOGFILE_FAILURE, successCode);
    if (successCode == 7) return; // if write failure to UDP then don't try again.  Otherwise, send error code by UDP
  }
  errorcode = endResponse(connection, successCode & 0xff);
  if (errorcode != DSE_OK)) {
    datastreamLogError(errorcode, 0);
    return;
  }
  
  ++nextEntryToSend;
  --numberOfEntriesLeftToSend;

  sendingLogData = (numberOfEntriesLeftToSend > 0);
}

// set up the datastream (EthernetLink already prepared)
void setupDataStream()
{
  sendingLogData = false;
}

/*
!s = request current sensor information
!p = request parameter information
!l{dword row nr}{word count} = request entries from log file
!n = request number of entries in log file
!c = cancel transmissions (log file)

response:
!{command letter echoed}{byte version} then:

for sensor:

for parameter:

for logfile:
!l -> the byte stream from the log file itself
!n -> dword number of entries in log file

The final byte in the packet is {byte status: 0 = ok, else error code}
Each response is a single UDP packet only.

*/
// execute the given command
DataStreamError executeDataStreamCommand(const char command[], int commandlength);
{
  bool commandIsValid = false;
  bool sendEndResponse = false;
  DataStreamError errorcode = DSE_OK;
  EthernetUDP *connection;

  if (commandLength >= 2 && command[0] == COMMAND_START_CHAR) {
    switch (command[1]) {
      case 's': {
        commandIsValid = true;
        break;
      }
      case 'p': {
        commandIsValid = true;
        break;
      }
      case 'n': {
        commandIsValid = true;
        unsigned long numberOfSamples = dataLogNumberOfSamples();

        errorcode = startResponse('n', &connection);
        if (errorcode == DSE_OK) {
          int byteswritten = connection->write((byte *)&numberOfSamples, sizeof(numberofSamples))
          if (byteswritten != sizeof(numberofSamples)) {
            errorcode = DSE_WRITE_FAILED;
          }
        }
        sendEndResponse = true;        
        break;
      }
      case 'c': {
        commandIsValid = true;
        sendingLogData = false;
        break;
      }
      case 'l': {
        commandIsValid = true;
        break;
      }
    }
  }

  if (!commandIsValid) {
    errorcode = DSE_INVALID_COMMAND_RECEIVED;
  } 
  if (errorcode != DSE_OK) {
    datastreamLogError(errorcode, 0);        
  }
  return errorcode;
}
