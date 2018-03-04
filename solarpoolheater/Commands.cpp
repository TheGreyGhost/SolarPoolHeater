#include "Commands.h"
#include "SystemStatus.h"
#include "Datalog.h"
#include "RealTimeClock.h"
#include "TemperatureProbes.h"
#include "SolarIntensity.h"
#include "PumpControl.h"

const int MAX_COMMAND_LENGTH = 30;
const int COMMAND_BUFFER_SIZE = MAX_COMMAND_LENGTH + 2;  // if buffer fills to max size, truncation occurs
int commandBufferIdx = -1;
char commandBuffer[COMMAND_BUFFER_SIZE];  
const char COMMAND_START_CHAR = '!';

// currently doesn't do anything in particular
void setupCommands()
{

  
}

// execute the command encoded in commandString.  Null-terminated
void executeCommand(char command[]) 
{
  bool commandIsValid = false;
  switch (command[0]) {
    case 'd': {commandIsValid = true; printDebugInfo(*console); break;}
    case '?': {
      commandIsValid = true;
      console->println("commands (turn CR+LF on):");
      console->println("!cr = read clock date+time");
      console->println("!cs Dec 26 2009 12:34:56 = set clock date + time (capitalisation, character count, and spacings must match exactly)");
      console->println("!d = print debug info");
      console->println("!le = erase log file");
      console->println("!li = log file info");
      console->println("!lr sample# count = read log data");
      console->println("!lv sample# count = view log data (more readable than lr)");
      console->println("!s = display solar readings and pump runtime");
      console->println("!t = toggle echo of temp probe readings");
      break;
    }
    case 't': {
      commandIsValid = true; 
      echoProbeReadings = !echoProbeReadings;
      if (echoProbeReadings) {
        for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
          console->print(probeNames[i]);
          console->print(":");
        }
        console->println();
      }  
      break;
    }
    case 's': {
      commandIsValid = true;
      console->print("solar intensity:"); 
        if (solarIntensityReadingInvalid) {
          console->println("INVALID");
        } else {
          console->println(smoothedSolarIntensity.getEWMA());
        }
      console->print("cumulative insolation:"); console->println(cumulativeInsolation);
      console->print("cumulative pump runtime (s):"); console->println(pumpRuntimeSeconds);
      break;
    }
    case 'c': {
      switch (command[1]) {
        case 'r': {
          commandIsValid = true;
          if (realTimeClockStatus) {
            printDateTime(*console, currentTime);
          } else {
            console->println("real time clock is not running");  
          }
          break;
        }
        case 's': {  //todo there is a bug in here eg Mar 04 2018 06:00:00 doesn't set time correctly.  !cs doesn't complain at all even if no args
          commandIsValid = true;
          if (strlen(command) < 3 + DATETIMEFORMAT_TOTALLENGTH) {
            console->println("syntax error.  !cs Dec 26 2009 12:34:56 (capitalisation, character count, and spacings must match exactly)");
          } else {
            DateTime newTime(command+3, command+3 + DATETIMEFORMAT_TIME_STARTPOS);
            console->print("setting date+time to ");
            printDateTime(*console, newTime);
            setDateTime(command+3); 
          } 
          break;
        }
      }
      break;
    }
    case 'l': {
      switch (command[1]) {
        case 'e': {             // le erase logfile
          commandIsValid = true; 
          bool success = dataLogErase();
          if (success) {
            console->println("data log erased successfully");
          } else {  
            console->println("failed to erase datafile for logged data");
          }
          break;
        }
        case 'i': { //li file info
          commandIsValid = true;
          unsigned long filesize = dataLogNumberOfSamples();
          console->print(filesize);
          console->println(" samples");
          break;
        }
        case 'v':      //lv sample# numberOfSamples - read
        case 'r': {    //lr sample# numberOfSamples - read
          commandIsValid = true;
          char *nextnumber;
          long arg1, arg2;
          arg1 = strtol(command + 2, &nextnumber, 10);
          arg2 = strtol(nextnumber, &nextnumber, 10);
          if (arg1 < 0 || arg2 <= 0) {
            console->println("invalid arguments");
          } else {
            dataLogExtractEntries(*console, arg1, arg2, (command[1] == 'r' ? "" : ": "));
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
    console->print("unknown command:");
    console->println(command);
    console->println("use ? for help");
  }
}

// look for incoming serial input (commands); collect the command and execute it when the entire command has arrived.
void tickCommands()
{
  while (consoleInput->available()) {
    if (commandBufferIdx < -1  || commandBufferIdx > COMMAND_BUFFER_SIZE) {
      assertFailureCode = ASSERT_INDEX_OUT_OF_BOUNDS;
      commandBufferIdx = -1;
    }
    int nextChar = consoleInput->read();
    if (nextChar == COMMAND_START_CHAR) {
      commandBufferIdx = 0;        
    } else if (nextChar == '\n') {
      if (commandBufferIdx == -1) {
        console->println("Type !? for help");
      } else if (commandBufferIdx > 0) {
        if (commandBufferIdx > MAX_COMMAND_LENGTH) {
          commandBuffer[MAX_COMMAND_LENGTH] = '\0';
          console->print("Command too long:"); console->println(commandBuffer);
        } else {
          commandBuffer[commandBufferIdx++] = '\0';
          executeCommand(commandBuffer);
        } 
        commandBufferIdx = -1; 
      }
    } else {
      if (commandBufferIdx >= 0 && commandBufferIdx < COMMAND_BUFFER_SIZE) {
        commandBuffer[commandBufferIdx++] = nextChar;
      }
    }
  }
}



