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

const int SD_CHIPSELECT = 4;
File datalogfile;
// each sample has timestamp; min, avg, max for each temp probe; cumulative insolation; cumulative pump runtime (s); averaged surge tank level; pumpState
const unsigned long DATALOG_BYTES_PER_SAMPLE = sizeof(long) + NUMBER_OF_PROBES * 3 * sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(PumpState);   //todo remove
const char DATALOG_FILENAME[] = "datalog.txt";

const char* logfileStatusText[4] = {"Card not present", "Failed to open", "Write failed", "OK"};

LogfileStatus logfileStatus;

const int LOG_PERIOD_SAMPLES = 60;

unsigned long lastLogTime;

void setupDatalog()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIPSELECT)) {
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

void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries, const char probeSeparator[])
{
  unsigned long filesize = datalogfile.size();
  unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;
  if (startidx >= samplesInFile) {
    dest.print("arg1 exceeds file size:");
    dest.println(samplesInFile);
  } else {
    datalogfile.seek(startidx * DATALOG_BYTES_PER_SAMPLE);
    long samplesToRead = min(numberOfEntries, min(24 * 60, samplesInFile - startidx)); // no more than one day at a time
    dest.print("timestamp(s) "); dest.print(probeSeparator);
    for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
      dest.print(probeNames[i]);
      dest.print(" min avg max ");
      dest.print(probeSeparator);
    }

    dest.print("cumul. insolation "); dest.print(probeSeparator);
    dest.print("surge tank avg level "); dest.print(probeSeparator);  //todo remove
    dest.print("cumul. pump runtime(s) "); dest.print(probeSeparator);
    dest.print("pump state "); dest.print(probeSeparator);
    dest.println();

    for (long i = 0; i < samplesToRead; ++i) {
      long timestamp;
      float cumulativeInsolation;
      float pumpRuntime;
      float surgeTankLevel;

      datalogfile.readBytes((byte *)&timestamp, sizeof(timestamp));
      dest.print(timestamp); dest.print(" "); dest.print(probeSeparator);

      for (int j = 0; j < NUMBER_OF_PROBES; ++j) {
        float temp[3];
        datalogfile.readBytes((byte *)temp, sizeof(temp));
        for (int k = 0; k < 3; ++k) {
          dest.print(temp[k], 1);
          dest.print(" ");
        }
        dest.print(probeSeparator);
      }

      datalogfile.readBytes((byte *)&cumulativeInsolation, sizeof(cumulativeInsolation));
      dest.print(cumulativeInsolation, 0); dest.print(" "); dest.print(probeSeparator);

      datalogfile.readBytes((byte *)&surgeTankLevel, sizeof(surgeTankLevel));      //todo remove
      dest.print(surgeTankLevel, 4); dest.print(" "); dest.print(probeSeparator);

      datalogfile.readBytes((byte *)&pumpRuntime, sizeof(pumpRuntime));
      dest.print(pumpRuntime, 0); dest.print(" "); // dest.print(probeSeparator);

      PumpState pumpState = getPumpState();
      datalogfile.readBytes((byte *)&pumpState, sizeof(pumpState));
      dest.print((int)pumpState, HEX); // dest.print(" "); // dest.print(probeSeparator);

      dest.println();
    }
  }
}
