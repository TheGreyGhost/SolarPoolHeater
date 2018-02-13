#include "Commands.h"
#include "SystemStatus.h"
#include "Datalog.h"
#include "TemperatureProbes.h"

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
      console->println("!d = print debug info");
      console->println("!t = toggle echo of probe readings");
      console->println("!le = erase log file");
      console->println("!li = log file info");
      console->println("!lr sample# count = read log data");
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
        case 'r': {    //lr sample# numberOfSamples - read
          commandIsValid = true;
          char *nextnumber;
          long arg1, arg2;
          arg1 = strtol(command + 2, &nextnumber, 10);
          arg2 = strtol(nextnumber, &nextnumber, 10);
          if (arg1 < 0 || arg2 <= 0) {
            console->println("invalid arguments");
          } else {
            dataLogExtractEntries(*console, arg1, arg2);
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



