#include "Datalog.h"
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <Wire.h>

const int SD_CHIPSELECT = 4;
File datalogfile;
const int DATALOG_BYTES_PER_SAMPLE = NUMBER_OF_PROBES * 3 * sizeof(float); // each sample has min, avg, max for each probe
const char DATALOG_FILENAME[] = "datalog.txt";

