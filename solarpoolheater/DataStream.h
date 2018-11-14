#ifndef DATASTREAM_H   
#define DATASTREAM_H  

const byte DATASTREAM_VERSION_BYTE = 'a';

enum DataStreamError {DSE_OK=0, DSE_INVALID_COMMAND_RECEIVED=1, DSE_RESPONSE_CONNECTION_FAILED=2, DSE_WRITE_FAILED=3, DSE_LOGFILE_FAILURE=4, DSE_ENDPACKET_FAILURE=5};

// execute the given command 
// DSE_OK for success, other = error
DataStreamError executeDataStreamCommand(const char command[], int commandlength);

// perform periodic update of the data stream
void tickDataStream();

// set up the datastream (EthernetLink already prepared)
void setupDataStream();

extern long dataStreamErrorCount;
extern DataStreamError lastDataStreamError;
extern int lastDataStreamErrorCode;

#endif
