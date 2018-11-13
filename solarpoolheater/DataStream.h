#ifndef DATASTREAM_H   
#define DATASTREAM_H  

// execute the given command
int executeCommand(const char command[]);

// perform periodic update of the data stream
void tickDataStream();

// set up the datastream (EthernetLink already prepared)
void setupDataStream();
#endif
