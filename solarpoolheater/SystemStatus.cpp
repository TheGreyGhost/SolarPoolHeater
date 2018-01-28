#include "SystemStatus.h"
#include "WatchDog.h"
#include "OutputDestination.h"
#include "SolarPoolHeater.h"
#include "TemperatureProbes.h"
#include "Datalog.h"

volatile byte assertFailureCode = 0;

Print *console;
Stream *consoleInput;

void printDebugInfo(Print &dest)
{
  dest.print("Version:"); dest.println(SPH_VERSION); 
  dest.print("Last Assert Error:"); dest.println(assertFailureCode); 
  dest.print("errorCountBusFailure:"); dest.println(errorCountBusFailure);
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    dest.println(probeNames[i]);
    dest.print("  errorCountNotFound:"); dest.println(errorCountNotFound[i]);
    dest.print("  errorCountCRCFailure:"); dest.println(errorCountCRCFailure[i]);
    dest.print("  errorCountImplausibleValue:"); dest.println(errorCountImplausibleValue[i]);
    dest.print("    errorLastImplausibleValue:"); dest.print(errorLastImplausibleValueRaw[i]);
    dest.print(" "); dest.println(errorLastImplausibleValueC[i]);
  }
}

const int MAX_ERROR_DEPTH = 16;
bool errorLEDstate;
long lastErrorLEDtime;
byte errorStack[MAX_ERROR_DEPTH];
byte errorStackIdx;
byte currentErrorBitpos;
byte currentErrorForLED;

void tickLEDstate();

void setupSystemStatus()
{
  console = new OutputDestinationSerial();
  consoleInput = &Serial;
  WatchDog::init(tickLEDstate);
  WatchDog::setPeriod(OVF_250MS);
  WatchDog::start();
}

void tickSystemStatus()
{
  //  the error display LED is ticked by ISR
}

void populateErrorStack()
{
  errorStackIdx = 0;
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    if (probeStatuses[i] != PS_OK) {
      errorStack[errorStackIdx++] = ERRORCODE_PROBE | i;
    }
  }
  if (logfileStatus != LFS_OK) {
    errorStack[errorStackIdx++] = ERRORCODE_DATALOG | logfileStatus;
  }
  if (assertFailureCode != 0) {
    errorStack[errorStackIdx++] = ERRORCODE_ASSERT | assertFailureCode;
  }
}

const byte PAUSE_BETWEEN_CODES = 8; // intervals of 250 ms
const byte BIT_SPACING = 4; // intervals of 250 ms
const byte ZERO_BIT_LENGTH = 1; // intervals of 250 ms
const byte ONE_BIT_LENGTH = 2; // intervals of 250 ms
const byte NO_ERROR_FLASH_LENGTH = 4; // intervals of 250 ms  LED ON, LED OFF,

enum LedState {IDLE, OK_OFF, OK_ON, ERROR_STACK_BIT_ON, ERROR_STACK_BIT_OFF, ERROR_STACK_BETWEEN_CODES} ledState = IDLE;
byte numberOfTicks = 0;
byte whichBit;
byte whichErrorEntry;

const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledONOFF = LOW;             // ledState used to set the LED

//ISR using the watchdog timer 

void tickLEDstate()
{
  LedState initialState;
  ++numberOfTicks;
  do {
    initialState = ledState;
    switch (ledState) {
      case IDLE: {
        populateErrorStack();
        if (errorStackIdx == 0) {
          ledState = OK_ON;
        } else {
          ledState = ERROR_STACK_BETWEEN_CODES;
          whichErrorEntry = 0;
        }
        break;
      }  
      case OK_OFF: {
        ledONOFF = LOW;      
        if (numberOfTicks >= NO_ERROR_FLASH_LENGTH) ledState = IDLE;
        break;
      }
      case OK_ON: {
        ledONOFF = HIGH;      
        if (numberOfTicks >= NO_ERROR_FLASH_LENGTH) ledState = OK_OFF;
        break;
      }
      case ERROR_STACK_BETWEEN_CODES: {
        ledONOFF = LOW;  
        if (numberOfTicks >= PAUSE_BETWEEN_CODES) {
          whichBit = 7;
          ledState = ERROR_STACK_BIT_ON;
        }
        break;
      }
      case ERROR_STACK_BIT_ON: {
        ledONOFF = HIGH;   
        bool one = errorStack[whichErrorEntry] & (1 << whichBit);
        if (numberOfTicks >= (one ? ONE_BIT_LENGTH : ZERO_BIT_LENGTH)) {
          ledState = ERROR_STACK_BIT_OFF;
        }
        break;
      }
      case ERROR_STACK_BIT_OFF: {
        ledONOFF = LOW;   
        bool one = errorStack[whichErrorEntry] & (1 << whichBit);
        if (numberOfTicks >= (one ? BIT_SPACING - ONE_BIT_LENGTH : BIT_SPACING - ZERO_BIT_LENGTH)) {
          if (whichBit == 0) {
            ++whichErrorEntry;
            if (whichErrorEntry >= errorStackIdx) {
              ledState = IDLE;
            } else {
             ledState = ERROR_STACK_BETWEEN_CODES;             
            }
          } else {
            --whichBit;
            ledState = ERROR_STACK_BIT_ON;  
          }
        }
        break;
      }
      default: assertFailureCode = ASSERT_INVALID_SWITCH; break;
    }
    if (initialState != ledState) numberOfTicks = 0;
  } while (initialState != ledState);
  digitalWrite(ledPin, ledONOFF);
  
}



