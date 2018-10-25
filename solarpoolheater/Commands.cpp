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

const int MAX_COMMAND_LENGTH = 30;
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
//      console->println("!da variable# = list all simulate-able variables");
//      console->println("!de string = echo string to ethernet port");

bool firstLetterD(const char *command) 
{
  bool commandIsValid = false;
    
  if (command[1] == '\0' || isspace(command[1])) {
    commandIsValid = true; 
    printDebugInfo(*console); 
  } else {
    switch (command[1])    {
      case 'a': {
        commandIsValid = true;
        SimVariables i;
        for (i = SIM_FIRST; i < SIM_LAST_PLUS_ONE; i = (SimVariables)((int)i + 1)) {
          console->print(getSimulationLabel(i));
          console->print("["); console->print((int)i); console->print("]:");
          if (isBeingSimulated(i)) {
            console->println(getSimulatedValue(i, -1.0));
          } else {
            console->println("-not simulated-");  
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
        console->print("Simulating ");
        console->print(getSimulationLabel((SimVariables)variable));
        console->print(" with ");
        console->println(getSimulatedValue((SimVariables)variable, -1.0));
        break;
      }
      case 'c': {
        const char *nextnumber;
        long variable; 
        float value;
        if (!parseLongFromString(command + 2, nextnumber, variable)) {
          if (*nextnumber == 'a') {
            stopSimulatingAll();
            console->println("stopped simulating all");
            commandIsValid = true;
          }
          break;
        }
        commandIsValid = true;
        stopSimulating((SimVariables)variable);
        console->print("Stopped simulating ");
        console->println(getSimulationLabel((SimVariables)variable));
        break;
      }
      case 'e': {
        commandIsValid = true;
        sendEthernetMessage(command+2, strlen(command) - 2);
        break;
      }
    }
  }  
  return commandIsValid;
}

//      console->println("!pr param# value = read EEPROM parameter");
//      console->println("!ps param# value = set EEPROM parameter.  if value = d, use default");
//      console->println("!pd = set all EEPROM parameter back to default");
//      console->println("!pa = list all EEPROM parameters");
//      console->println("!p? = list EEPROM parameter names");
bool firstLetterP(const char *command) 
{
  bool commandIsValid = false;
 
  switch (command[1]) {
    case 'd': {
      commandIsValid = true;
      setSettingDefaultAll();
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        console->print("The value of ");
        console->print(getEEPROMSettingLabel(i));
        console->print("[");  
        console->print(i);
        console->print("] was set to ");
        console->println(getSetting(i));
      }  
      break;
    }
    case 'a': {
      commandIsValid = true;
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        console->print(i);
        console->print(": ");            
        console->print(getEEPROMSettingLabel(i));
        console->print(": ");
        console->println(getSetting(i));
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
      console->print("The value of ");
      console->print(getEEPROMSettingLabel(whichSetting));
      console->print("[");  
      console->print(whichSetting);
      console->print("] is ");
      console->println(value);
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
      console->print("The value of ");
      console->print(getEEPROMSettingLabel(whichSetting));
      console->print("[");  
      console->print(whichSetting);
      console->print("] was set to ");
      console->println(value);         
      commandIsValid = true;
      break;
    }
    case '?':{
      commandIsValid = true;
      EEPROMSettings i;
      for (i = SET_FIRST; i < SET_INVALID; i = (EEPROMSettings)((int)i + 1)) {
        console->print((int)i);
        console->print(" = ");
        console->println(getEEPROMSettingLabel(i));
      }
      break;
    }
  }
  return commandIsValid;
}

// execute the command encoded in commandString.  Null-terminated
void executeCommand(char command[]) 
{
  bool commandIsValid = false;
  switch (command[0]) {
    case '?': {
      commandIsValid = true;
      console->println("commands (turn CR+LF on):");
      console->println("!cr = read clock date+time");
      console->println("!cs Dec 26 2009 12:34:56 = set clock date + time (capitalisation, character count, and spacings must match exactly)");
      console->println("!d = print debug info");
      console->println("!da variable# = list all simulate-able variables");
      console->println("!dc variable# = cancel simulation for variable #, if variable# = a then cancel all");
      console->println("!ds variable# value = simulate variable# with value");
      console->println("!de string = echo string to ethernet port");
      console->println("!le = erase log file");
      console->println("!li = log file info");
      console->println("!lr sample# count = read log data");
      console->println("!lv sample# count = view log data (more readable than lr)");
      console->println("!pr param# = read EEPROM parameter");
      console->println("!ps param# value = set EEPROM parameter.  if value = d, use default");
      console->println("!pd = set all EEPROM parameter back to default");
      console->println("!pa = read all EEPROM parameters");
      console->println("!p? = list EEPROM parameter names");
      console->println("!s = display solar readings and pump runtime");
      console->println("!t = toggle echo of temp probe readings");
      break;
    }
    case 'd': {
      commandIsValid = firstLetterD(command);
      break;
    }
    case 'p': {
      commandIsValid = firstLetterP(command);
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
      console->print("surge tank level:"); console->println(surgeTankLevelOK ? "OK" : "LOW");
      console->print("pump state:");
      console->print(getCurrentPumpStateLabel());
      console->print("["); console->print(getPumpState()); console->println("]");
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
        case 's': {  
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
