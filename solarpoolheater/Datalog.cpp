#include "Datalog.h"
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DallasTemperatureErrorCodes.h>

#include "RealTimeClock.h"
#include "SolarIntensity.h"
#include "PumpControl.h"
#include "TemperatureProbes.h"
#include "Settings.h"
#include "PinAssignments.h"

File datalogfile;
// each sample has timestamp; min, avg, max for each temp probe; cumulative insolation; cumulative pump runtime (s); averaged surge tank level; pumpState
const unsigned long DATALOG_BYTES_PER_SAMPLE = sizeof(long) + NUMBER_OF_PROBES * 3 * sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(PumpState);   //todo remove
const char DATALOG_FILENAME[] = "datalog.txt";

const char* logfileStatusText[4] = {"OK", "Card not present", "Failed to open", "Write failed"};

LogfileStatus logfileStatus;

const int LOG_PERIOD_SAMPLES = 60;

unsigned long lastLogTime;

void setupDatalog()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(DIGPIN_CHIPSELECT_SDCARD)) {
    logfileStatus = LFS_CARD_NOT_PRESENT;
    return;
  }
  logfileStatus = LFS_OK;
  //  setupEthernet();

  datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
  if (!datalogfile) {
    logfileStatus = LFS_FAILED_TO_OPEN;
  }
  lastLogTime = millis();
}

void tickDatalog()
{
  unsigned long secondsSinceLastLog = (millis() - lastLogTime) / 1000;
  if (secondsSinceLastLog >= (isSystemIdle() ?  getSetting(SET_logIntervalIdleSeconds) :  getSetting(SET_logIntervalSeconds))) {
    lastLogTime = millis();
    // if the file is available, write to it:
    if (datalogfile) {  // don't forget to update DATALOG_BYTES_PER_SAMPLE
      datalogfile.seek(datalogfile.size());

      size_t totalBytesWritten = 0;
      long timestamp = currentTime.secondstime();
      size_t bytesWritten = datalogfile.write((byte *)&timestamp, sizeof timestamp);
      if (bytesWritten != sizeof timestamp) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      totalBytesWritten += bytesWritten;

      for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
        float temp[3];
        if (probeStatuses[i] == PS_OK) {
          temp[0] = temperatureDataStats[i].getMin();
          temp[1] = temperatureDataStats[i].getAverage();
          temp[2] = temperatureDataStats[i].getMax();
        } else {
          temp[0] = 0.0; temp[1] = 0.0; temp[2] = 0.0;
        }
        bytesWritten = datalogfile.write((byte *)temp, sizeof temp);
        if (bytesWritten != sizeof temp) {
          logfileStatus = LFS_WRITE_FAILED;
        }
        totalBytesWritten += bytesWritten;
        temperatureDataStats[i].clear();
      }

      bytesWritten = datalogfile.write((byte *)&cumulativeInsolation, sizeof cumulativeInsolation);
      if (bytesWritten != sizeof cumulativeInsolation) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      totalBytesWritten += bytesWritten;

      // todo remove
      float tempLevel = surgeTankLevelStats.getAverage();
      surgeTankLevelStats.clear();
      bytesWritten = datalogfile.write((byte *)&tempLevel, sizeof tempLevel);
      if (bytesWritten != sizeof tempLevel) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      totalBytesWritten += bytesWritten;

      bytesWritten = datalogfile.write((byte *)&pumpRuntimeSeconds, sizeof pumpRuntimeSeconds);
      if (bytesWritten != sizeof pumpRuntimeSeconds) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      totalBytesWritten += bytesWritten;

      PumpState pumpState = getPumpState();
      bytesWritten = datalogfile.write((byte *)&pumpState, sizeof pumpState);
      if (bytesWritten != sizeof pumpState) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      totalBytesWritten += bytesWritten;

      if (totalBytesWritten != DATALOG_BYTES_PER_SAMPLE) {
        logfileStatus = LFS_WRITE_FAILED;
      }
      datalogfile.flush();
    }
  }
}

bool dataLogErase()
{
  bool success;
  datalogfile.close();
  success = SD.remove(DATALOG_FILENAME);
  if (success) {
    datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
    if (!datalogfile) {
      success = false;
    }
  }
  return success;
}

unsigned long dataLogNumberOfSamples()
{
  return datalogfile.size() / DATALOG_BYTES_PER_SAMPLE;
}

unsigned long timestamps[30]; //todo remove
int tsidx;
void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries, const char probeSeparator[])
{
  unsigned long filesize = datalogfile.size();
  unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;

  tsidx = 0;
  
  if (startidx >= samplesInFile) {
    dest.print("arg1 exceeds file size:");
    dest.println(samplesInFile);
  } else {
    timestamps[0] = micros();  //todo remove
    datalogfile.seek(startidx * DATALOG_BYTES_PER_SAMPLE);
    timestamps[1] = micros();  //todo remove
    long samplesToRead = min(numberOfEntries, min(24 * 60, samplesInFile - startidx)); // no more than one day at a time
    timestamps[2] = micros();  //todo remove
    dest.print("timestamp(s) "); dest.print(probeSeparator);
    timestamps[3] = micros();  //todo remove
    for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
      dest.print(probeNames[i]);
      dest.print(" min avg max ");
      dest.print(probeSeparator);
    }
    timestamps[4] = micros();  //todo remove
    dest.print("cumul. insolation "); dest.print(probeSeparator);
    dest.print("surge tank avg level "); dest.print(probeSeparator);  //todo remove
    dest.print("cumul. pump runtime(s) "); dest.print(probeSeparator);
    dest.print("pump state "); dest.print(probeSeparator);
    dest.println();
    timestamps[5] = micros();  //todo remove

    for (long i = 0; i < samplesToRead; ++i) {
      long timestamp;
      float cumulativeInsolation;
      float pumpRuntime;
      float surgeTankLevel;

      timestamps[6] = micros();  //todo remove
      datalogfile.readBytes((byte *)&timestamp, sizeof(timestamp));
      timestamps[7] = micros();  //todo remove
      dest.print(timestamp); dest.print(" "); dest.print(probeSeparator);
      timestamps[8] = micros();  //todo remove

      for (int j = 0; j < NUMBER_OF_PROBES; ++j) {
        float temp[3];
        timestamps[9] = micros();  //todo remove
        datalogfile.readBytes((byte *)temp, sizeof(temp));
        timestamps[10] = micros();  //todo remove
        for (int k = 0; k < 3; ++k) {
          dest.print(temp[k], 1);
          dest.print(" ");
        }
        timestamps[11] = micros();  //todo remove
        dest.print(probeSeparator);
      }

      timestamps[12] = micros();  //todo remove
      datalogfile.readBytes((byte *)&cumulativeInsolation, sizeof(cumulativeInsolation));
      timestamps[13] = micros();  //todo remove
      dest.print(cumulativeInsolation, 0); dest.print(" "); dest.print(probeSeparator);

      timestamps[14] = micros();  //todo remove
      datalogfile.readBytes((byte *)&surgeTankLevel, sizeof(surgeTankLevel));      //todo remove
      timestamps[15] = micros();  //todo remove
      dest.print(surgeTankLevel, 4); dest.print(" "); dest.print(probeSeparator);

      timestamps[16] = micros();  //todo remove
      datalogfile.readBytes((byte *)&pumpRuntime, sizeof(pumpRuntime));
      timestamps[17] = micros();  //todo remove
      dest.print(pumpRuntime, 0); dest.print(" "); // dest.print(probeSeparator);

      timestamps[18] = micros();  //todo remove
      PumpState pumpState = getPumpState();
      timestamps[19] = micros();  //todo remove
      datalogfile.readBytes((byte *)&pumpState, sizeof(pumpState));
      dest.print((int)pumpState, HEX); // dest.print(" "); // dest.print(probeSeparator);
      timestamps[20] = micros();  //todo remove

      dest.println();
      timestamps[21] = micros();  //todo remove
    }
  }
  for (int i =  1; i < 22; ++i) {
    Serial.print(i);
    Serial.print(":");
    Serial.println(timestamps[i] - timestamps[i-1]);
  }
}

// print the given entry to dest, in the format of raw bytes from the log file
// returns 0 for success or other for failure code
int dataLogPrintEntryBytes(Print &dest, long startidx)
{
  if (logfileStatus != LFS_OK) {
    return logfileStatus;  
  }
  unsigned long filesize = datalogfile.size();
  unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;
  if (startidx >= samplesInFile) {
    return 4;
  } 

  byte buffer[DATALOG_BYTES_PER_SAMPLE];
    
  bool success = datalogfile.seek(startidx * DATALOG_BYTES_PER_SAMPLE);
  if (!success) {
    return 8;
  }

 /* For success read() returns the number of bytes read.
 * A value less than nbyte, including zero, will be returned
 * if end of file is reached.
 * If an error occurs, read() returns -1.  Possible errors include
 * read() called before a file has been opened, corrupt file system
 * or an I/O error occurred.
 */
  int bytesread = datalogfile.readBytes(buffer, sizeof(buffer));
  if (bytesread == -1) return 5;
  if (bytesread != DATALOG_BYTES_PER_SAMPLE) return 6;
  bytesread = dest.write(buffer, DATALOG_BYTES_PER_SAMPLE);
  if (bytesread != DATALOG_BYTES_PER_SAMPLE) return 7;
  return 0;
}
