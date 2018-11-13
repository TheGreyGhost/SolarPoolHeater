#include "DataStream.h"

// execute the given command
int executeCommand(const char command[]);

bool sendingLogData;
long nextEntryToSend;
long numberOfEntriesLeftToSend;

// perform periodic update of the data stream
void tickDataStream()
{
  if (ethernetStatus != ES_OK) return;
  if (sendingLogData) {
    EthernetUDP &connection = prepareEthernetDatastreamMessage();
    int success = dataLogRetrieveEntryBytes(connection, nextEntryToSend);

    connection.write(success & 0xff);
    connection.endPacket();
    ++nextEntryToSend;
    --numberOfEntriesLeftToSend;
    sendingLogData = (numberOfEntriesLeftToSend > 0);
  }
}

// set up the datastream (EthernetLink already prepared)
void setupDataStream()
{
  sendingLogData = false;
}

