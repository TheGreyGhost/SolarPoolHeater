#include <ctype.h>
#include "Commands.h"
#include "SystemStatus.h"
#include "Datalog.h"
#include "RealTimeClock.h"
#include "TemperatureProbes.h"
#include "SolarIntensity.h"
#include "PumpControl.h"
#include "Simulate.h"
#include "Settings.h"
#include "EthernetLink.h"
#include "DebugTests.h"

const int COMMAND_BUFFER_SIZE = MAX_COMMAND_LENGTH + 2;  // if buffer fills to max size, truncation occurs
int commandBufferIdx = -1;
char commandBuffer[COMMAND_BUFFER_SIZE];  
const char COMMAND_START_CHAR = '!';

// currently doesn't do anything in particular
void setupCommands()
{

  
}

// parse a long from the given string, returns in retval.  Also returns the ptr to the next character which wasn't parsed
// returns false if no valid number found (without altering retval)
bool parseLongFromString(const char *buffer, const char * &nextUnparsedChar, long &retval)
{
  while (isspace(*buffer)) {
    ++buffer;
  }
  nextUnparsedChar = buffer;
  if (!isdigit(*buffer) && *buffer != '-') return false;
  char *forceNonConst = (char *)nextUnparsedChar;
  retval = strtol(buffer, &forceNonConst, 10);
  nextUnparsedChar = (const char *)forceNonConst;
  return true;
}

// parse a long from the given string, returns in retval.  Also returns the ptr to the next character which wasn't parsed
// returns false if no valid number found (without altering retval)
bool parseFloatFromString(const char *buffer, const char * &nextUnparsedChar, float &retval)
{
  while (isspace(*buffer)) {
    ++buffer;
  }
  nextUnparsedChar = buffer;
  if (!isdigit(*buffer) && *buffer != '-') return false;
  char *forceNonConst = (char *)nextUnparsedChar;
  retval = (float)strtod(buffer, &forceNonConst);
  nextUnparsedChar = (const char *)forceNonConst;
  return true;
}

//      console->println("!d = print debug info");
//      console->println("!ds variable# value = simulate variable# with value");
//      console->println("!dc variable# = cancel simulation for variable #, if variable# = a then cancel all");
//      console->println("!da = list all simulate-able variables");
//      console->println("!de string = echo string to ethernet port");
//      console->println("!dt test# = execute debug test #");

bool firstLetterD(const char *command, Print *replyConsole) 
{
  bool commandIsValid = false;
    
  if (command[1] == '\0' || isspace(command[1])) {
    commandIsValid = true; 
    printDebugInfo(*replyConsole); 
  } else {
    switch (command[1])    {
      case 'a': {
        commandIsValid = true;
        SimVariables i;
        for (i = SIM_FIRST; i < SIM_LAST_PLUS_ONE; i = (SimVariables)((int)i + 1)) {
          replyConsole->print(getSimulationLabel(i));
          replyConsole->print("["); replyConsole->print((int)i); replyConsole->print("]:");
          if (isBeingSimulated(i)) {
            replyConsole->println(getSimulatedValue(i, -1.0));
          } else {
            replyConsole->println("-not simulated-");  
          }
        }
        break;
      }

      case 's': {
        const char *nextnumber;
        long variable; 
        float value;
        if (!parseLongFromString(command + 2, nextnumber, variable)) {
          break;
        }
        if (!parseFloatFromString(nextnumber, nextnumber, value)) {
          break;
        }
        commandIsValid = true;
        setSimulatedValue((SimVariables)variable, value);
        replyConsole->print("Simulating ");
        replyConsole->print(getSimulationLabel((SimVariables)variable));
        replyConsole->print(" with ");
        replyConsole->println(getSimulatedValue((SimVariables)variable, -1.0));
        break;
      }
      case 'c': {
        const char *nextnumber;
        long variable; 
        float value;
        if (!parseLongFromString(command + 2, nextnumber, variable)) {
          if (*nextnumber == 'a') {
            stopSimulatingAll();
            replyConsole->println("stopped simulating all");
            commandIsValid = true;
          }
          break;
        }
        commandIsValid = true;
        stopSimulating((SimVariables)variable);
        replyConsole->print("Stopped simulating ");
        replyConsole->println(getSimulationLabel((SimVariables)variable));
        break;
      }

      case 't': {
        const char *nextnumber;
        long variable; 
        float value;
        if (!parseLongFromString(command + 2, nextnumber, variable)) {
          break;
        }
        commandIsValid = true;
        runTest((int)variable);
        break;
      }

      case 'e': {
        commandIsValid = true;
        sendEthernetTerminalMessage((byte *)(command+2), strlen(command) - 2);
        break;
      }
    }
  }  
  return commandIsValid;
}

//      replyConsole->println("!pr param# value = read EEPROM parameter");
//      replyConsole->println("!ps param# value = set EEPROM parameter.  if value = d, use default");
//      replyConsole->println("!pd = set all EEPROM parameter back to default");
//      replyConsole->println("!pa = list all EEPROM parameters");
//      replyConsole->println("!p? = list EEPROM parameter names");
bool firstLetterP(const char *command, Print *replyConsole)  
{
  bool commandIsValid = false;
 
  switch (command[1]) {
    case 'd': {
      commandIsValid = true;
      setSettingDefaultAll();
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        replyConsole->print("The value of ");
        replyConsole->print(getEEPROMSettingLabel(i));
        replyConsole->print("[");  
        replyConsole->print(i);
        replyConsole->print("] was set to ");
        replyConsole->println(getSetting(i));
      }  
      break;
    }
    case 'a': {
      commandIsValid = true;
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        replyConsole->print(i);
        replyConsole->print(": ");            
        replyConsole->print(getEEPROMSettingLabel(i));
        replyConsole->print(": ");
        replyConsole->println(getSetting(i));
      }  
      break;
    }
    case 'r':{
      const char *nextnumber;
      long param; 
      float value;
      if (!parseLongFromString(command + 2, nextnumber, param)) {
        break;
      }
      commandIsValid = true;
      EEPROMSettings whichSetting = (EEPROMSettings)param;
      value = getSetting(whichSetting);
      replyConsole->print("The value of ");
      replyConsole->print(getEEPROMSettingLabel(whichSetting));
      replyConsole->print("[");  
      replyConsole->print(whichSetting);
      replyConsole->print("] is ");
      replyConsole->println(value);
      break;
    }
    case 's':{
      const char *nextnumber;
      long param; 
      float value;
      if (!parseLongFromString(command + 2, nextnumber, param)) {
        break;
      }
      EEPROMSettings whichSetting = (EEPROMSettings)param;
      if (!parseFloatFromString(nextnumber, nextnumber, value)) {
        if (*nextnumber != 'd') {
          break;
        }
        value = setSettingDefault(whichSetting);
      } else {
        value = setSetting(whichSetting, value);
      }
      replyConsole->print("The value of ");
      replyConsole->print(getEEPROMSettingLabel(whichSetting));
      replyConsole->print("[");  
      replyConsole->print(whichSetting);
      replyConsole->print("] was set to ");
      replyConsole->println(value);         
      commandIsValid = true;
      break;
    }
    case '?':{
      commandIsValid = true;
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        replyConsole->print((int)i);
        replyConsole->print(" = ");
        replyConsole->println(getEEPROMSettingLabel(i));
      }
      break;
    }
  }
  return commandIsValid;
}

// execute the command encoded in commandString.  Null-terminated
void executeCommand(char command[], Print *replyConsole)  
{
  bool commandIsValid = false;
  switch (command[0]) {
    case '?': {
      commandIsValid = true;
      replyConsole->println(F("commands (turn CR+LF on):"));
      replyConsole->println(F("!cr = read clock date+time"));
      replyConsole->println(F("!cs Dec 26 2009 12:34:56 = set clock date + time (capitalisation, character count, and spacings must match exactly)"));
      replyConsole->println(F("!d = print debug info"));
      replyConsole->println(F("!da = list all simulate-able variables"));
      replyConsole->println(F("!dc variable# = cancel simulation for variable #, if variable# = a then cancel all"));
      replyConsole->println(F("!ds variable# value = simulate variable# with value"));
      replyConsole->println(F("!de string = echo string to ethernet port"));
      replyConsole->println(F("!dt test# = execute debug test #"));
      replyConsole->println(F("!le = erase log file"));
      replyConsole->println(F("!li = log file info"));
      replyConsole->println(F("!lr sample# count = read log data"));
      replyConsole->println(F("!lv sample# count = view log data (more readable than lr)"));
      replyConsole->println(F("!pr param# = read EEPROM parameter"));
      replyConsole->println(F("!ps param# value = set EEPROM parameter.  if value = d, use default"));
      replyConsole->println(F("!pd = set all EEPROM parameter back to default"));
      replyConsole->println(F("!pa = read all EEPROM parameters"));
      replyConsole->println(F("!p? = list EEPROM parameter names"));
      replyConsole->println(F("!s = display solar readings and pump runtime"));
      replyConsole->println(F("!sr = reset pump status"));
      replyConsole->println(F("!t = toggle echo of temp probe readings"));
      break;
    }
    case 'd': {
      commandIsValid = firstLetterD(command, replyConsole);
      break;
    }
    case 'p': {
      commandIsValid = firstLetterP(command, replyConsole);
      break;
    }
    case 't': {
      commandIsValid = true; 
      echoProbeReadings = !echoProbeReadings;
      if (echoProbeReadings) {
        for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
          replyConsole->print(probeNames[i]);
          replyConsole->print(":");
        }
        replyConsole->println();
      }  
      break;
    }
    case 's': {
      int k = command[1];
      replyConsole->println(k);
      switch (command[1]) {
        case 'r': {
          commandIsValid = true;
          resetPumpErrors();
          break;
        }

        case '\0': {
          commandIsValid = true;
          replyConsole->print("solar intensity:"); 
            if (solarIntensityReadingInvalid) {
              replyConsole->println("INVALID");
            } else {
              replyConsole->println(smoothedSolarIntensity.getEWMA());
            }
          replyConsole->print("cumulative insolation:"); replyConsole->println(cumulativeInsolation);
          replyConsole->print("surge tank level:"); replyConsole->println(surgeTankLevelOK ? "OK" : "LOW");
          replyConsole->print("pump state:");
          replyConsole->print(getCurrentPumpStateLabel());
          replyConsole->print("["); replyConsole->print(getPumpState()); replyConsole->println("]");
          replyConsole->print("cumulative pump runtime (s):"); replyConsole->println(pumpRuntimeSeconds);
          break;
        }  
      }
      break;
    }
    case 'c': {
      switch (command[1]) {
        case 'r': {
          commandIsValid = true;
          if (realTimeClockStatus) {
            printDateTime(*console, currentTime);
          } else {
            replyConsole->println("real time clock is not running");  
          }
          break;
        }
        case 's': {  
          commandIsValid = true;
          if (strlen(command) < 3 + DATETIMEFORMAT_TOTALLENGTH) {
            replyConsole->println(F("syntax error.  !cs Dec 26 2009 12:34:56 (capitalisation, character count, and spacings must match exactly)"));
          } else {
            DateTime newTime(command+3, command+3 + DATETIMEFORMAT_TIME_STARTPOS);
            replyConsole->print("setting date+time to ");
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
            replyConsole->println("data log erased successfully");
          } else {  
            replyConsole->println(F("failed to erase datafile for logged data"));
          }
          break;
        }
        case 'i': { //li file info
          commandIsValid = true;
          unsigned long filesize = dataLogNumberOfSamples();
          replyConsole->print(filesize);
          replyConsole->println(" samples");
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
            replyConsole->println("invalid arguments");
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
    replyConsole->print("unknown command:");
    replyConsole->println(command);
    replyConsole->println("use ? for help");
  }
}

// look for incoming serial input (commands); collect the command and execute it when the entire command has arrived.
void tickCommands()
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
      console = serialConsole;
      if (commandBufferIdx == -1) {
        console->println("Type !? for help");
      } else if (commandBufferIdx > 0) {
        if (commandBufferIdx > MAX_COMMAND_LENGTH) {
          commandBuffer[MAX_COMMAND_LENGTH] = '\0';
          console->print("Command too long:"); console->println(commandBuffer);
        } else {
          commandBuffer[commandBufferIdx++] = '\0';
          executeCommand(commandBuffer, serialConsole);
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

void parseIncomingInput(char command[], int bufferlen, Print *replyConsole)
{
  char *end, *shorter;
  if (bufferlen < 1 || command[0] != COMMAND_START_CHAR) {
    replyConsole->println("Type !? for help");
  } else {
    end = (char *)memchr(command, '\0', bufferlen);
    if (NULL == end) {
      replyConsole->println(F("Program error: missing terminating null"));
      return;
    }
    shorter = (char *)strpbrk(command, "\n\r"); // find first null, 0x0a or 0x0d
    if (shorter != NULL) end = shorter;
    if (end - command > MAX_COMMAND_LENGTH) {
      replyConsole->print("Command too long:"); replyConsole->println(command);    
    } else {
      executeCommand(command+1, replyConsole);    
    }
  }
}
