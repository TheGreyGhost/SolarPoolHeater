#include "Commands.h"
const int MAX_COMMAND_LENGTH = 30;
const int COMMAND_BUFFER_SIZE = MAX_COMMAND_LENGTH + 2;  // if buffer fills to max size, truncation occurs
int commandBufferIdx = -1;
char commandBuffer[COMMAND_BUFFER_SIZE];  
const char COMMAND_START_CHAR = '!';

// execute the command encoded in commandString.  Null-terminated
void executeCommand(char command[]) 
{
  bool commandIsValid = false;
  //Serial.print("Execute command:"); Serial.println(command);  
  switch (command[0]) {
    case 'd': {commandIsValid = true; printDebugInfo(); break;}
    case '?': {
      commandIsValid = true;
      Serial.println("commands (turn CR+LF on):");
      Serial.println("!d = print debug info");
      Serial.println("!t = toggle echo of probe readings");
      Serial.println("!le = erase log file");
      Serial.println("!li = log file info");
      Serial.println("!lr sample# count = read log data");
      break;
    }
    case 't': {
      commandIsValid = true; 
      echoProbeReadings = !echoProbeReadings;
      break;
    }
    case 'l': {
      switch (command[1]) {
        case 'e': {             // le erase logfile
          commandIsValid = true; 
          bool success;
          datalogfile.close();
          success = SD.remove(DATALOG_FILENAME);
          if (success) {
            datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
            if (datalogfile) {
              Serial.println("data log erased successfully");
            } else {
              success = false;
            }
          }
          if (!success) {
            Serial.println("failed to erase datafile for logged data");
          }
          break;
        }
        case 'i': { //li file info
          commandIsValid = true;
          unsigned long filesize = datalogfile.size();
          Serial.print(filesize / DATALOG_BYTES_PER_SAMPLE);
          Serial.println(" samples");
          break;
        }
        case 'r': {    //lr sample# numberOfSamples - read
          commandIsValid = true;
          char *nextnumber;
          long arg1, arg2;
          arg1 = strtol(command + 2, &nextnumber, 10);
          arg2 = strtol(nextnumber, &nextnumber, 10);
          if (arg1 < 0 || arg2 <= 0) {
            Serial.println("invalid arguments");
          } else {
            unsigned long filesize = datalogfile.size();
            unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;
            if (arg1 >= samplesInFile) {
               Serial.print("arg1 exceeds file size:");
               Serial.println(samplesInFile);
            } else {  
              datalogfile.seek(arg1 * DATALOG_BYTES_PER_SAMPLE);
              long samplesToRead = min(arg2, min(24*60, samplesInFile - arg1));  // something wrong here?  asked for 2530 but only got 1420?
              for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
                Serial.print(probeNames[i]); 
                Serial.print(" min avg max ");
              }
              Serial.println();
              for (long i = 0; i < samplesToRead; ++i) {
                for (int j = 0; j < NUMBER_OF_PROBES; ++j) {
                  float temp[3];
                  for (int k = 0; k < sizeof(temp); ++k) {
                    ((byte *)temp)[k] = datalogfile.read(); 
                  }
                  for (int k = 0; k < 3; ++k) {
                    Serial.print(temp[k], 1); 
                    Serial.print(" ");
                  }  
                }
                Serial.println();
              }
            }
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
    Serial.print("unknown command:");
    Serial.println(command);
    Serial.println("use ? for help");
  }
}

// look for incoming serial input (commands); collect the command and execute it when the entire command has arrived.
void processIncomingSerial()
{
  while (Serial.available()) {
    if (commandBufferIdx < -1  || commandBufferIdx > COMMAND_BUFFER_SIZE) {
      assertFailureCode = ASSERT_INDEX_OUT_OF_BOUNDS;
      commandBufferIdx = -1;
    }
    int nextChar = Serial.read();
    if (nextChar == COMMAND_START_CHAR) {
      commandBufferIdx = 0;        
    } else if (nextChar == '\n') {
      if (commandBufferIdx == -1) {
        Serial.println("Type !? for help");
      } else if (commandBufferIdx > 0) {
        if (commandBufferIdx > MAX_COMMAND_LENGTH) {
          commandBuffer[MAX_COMMAND_LENGTH] = '\0';
          Serial.print("Command too long:"); Serial.println(commandBuffer);
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



