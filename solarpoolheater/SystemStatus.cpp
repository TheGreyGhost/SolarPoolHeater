#include "SystemStatus.h"
#include <DigitalIO.h>
#include "WatchDog.h"
#include "OutputDestination.h"
#include "SolarPoolHeater.h"
#include "TemperatureProbes.h"
#include "Datalog.h"
#include "RealTimeClock.h"
#include "SolarIntensity.h"
#include "PumpControl.h"
#include "EthernetLink.h"
#include "PinAssignments.h"

byte assertFailureCode = 0;

Print *console;
Print *serialConsole;
//Stream *consoleInput;

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
  dest.print("real time clock is running:"); dest.println(realTimeClockStatus);
  dest.print("log file status:");
  if (logfileStatus >= 0 && logfileStatus <= LFS_OK) {
    dest.println(logfileStatusText[logfileStatus]);
  } else {
    dest.println(logfileStatus);
  }  
  dest.print("Ethernet status:");
  if (ethernetStatus >= 0 && ethernetStatus <= ES_OK) {
    dest.println(ethernetStatusText[ethernetStatus]);
  } else {
    dest.println(ethernetStatus);
  }  
  dest.print("solar intensity sensor status:");
  dest.print(solarIntensityReadingInvalid ? "INVALID " : "OK");
  dest.print(" with lastInvalidReading:");
  dest.println(lastInvalidReading);
}

DigitalPin<DIGPIN_STATUS_LED> pinStatusLED;

const int MAX_ERROR_DEPTH = 16;
byte errorStack[MAX_ERROR_DEPTH];
byte errorStackIdx = 0;

void updateStatusLEDisr();

void setupSystemStatus()
{
  pinStatusLED.mode(OUTPUT);
  serialConsole = new OutputDestinationSerial();
  console = serialConsole;
//  consoleInput = &Serial;
  WatchDog::init(updateStatusLEDisr);
  WatchDog::setPeriod(OVF_250MS);
  WatchDog::start();
}

void tickStatusLEDsequence();

void tickSystemStatus()
{
  tickStatusLEDsequence();
}

bool shutdownErrorsPresent()
{
  for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
    if (probeStatuses[i] != PS_OK) {
      return true;
    }
  }
  if (logfileStatus != LFS_OK) {
    return true;
  }
  if (assertFailureCode != 0) {
    return true;
  }
  if (!realTimeClockStatus) {
    return true;
  }

  return false;
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
  if (ethernetStatus != ES_OK) {
    errorStack[errorStackIdx++] = ERRORCODE_ETHERNET | ethernetStatus;
  }
  if (assertFailureCode != 0) {
    errorStack[errorStackIdx++] = ERRORCODE_ASSERT | assertFailureCode;
  }
  if (!realTimeClockStatus) {
    errorStack[errorStackIdx++] = ERRORCODE_RTC;
  }
  if (solarIntensityReadingInvalid) {
    errorStack[errorStackIdx++] = ERRORCODE_SOLAR_SENSOR;
  }
  if (isPumpInError()) {
    errorStack[errorStackIdx++] = ERRORCODE_PUMP_CONTROL | getPumpErrorCode();
  }  
}

const byte PAUSE_BETWEEN_CODES = 8; // intervals of 250 ms
const byte BIT_SPACING = 4; // intervals of 250 ms
const byte ZERO_BIT_LENGTH = 1; // intervals of 250 ms
const byte ONE_BIT_LENGTH = 2; // intervals of 250 ms
const byte NO_ERROR_FLASH_LENGTH = 4; // intervals of 250 ms  LED ON, LED OFF,

enum LedState {OK, ERROR_STACK, ERROR_STACK_BETWEEN_CODES} ledState = OK;
byte whichErrorEntry = 0;

// the sequences are shifted out MSB first, 0 = LED on, 1 = LED off.  Each bit is 250 ms, so unsigned long is 8 seconds.  The sequence stops when it is all zeros (so the last bit in the flash sequence is always a 1, 
//   corresponding to LED off
const unsigned long SEQ_BETWEEN_CODES = 0xff000000; // 2 second off
const unsigned long SEQ_NO_ERROR = 0x0f000000; // 2 seconds: 1 second off, 1 second on

const byte ZERO_BIT_CODE = 0x07; // 250 ms on, 750 ms off
const byte ONE_BIT_CODE = 0x01;  // 750 ms on, 250 ms off

volatile unsigned long flashSequence;  // each bit corresponds to a 250 ms window: 1 = LED off, 0 = LED on.  shifted out MSB first.  When flashSequence == 0, ready for next one

// The ISR actually alters the LED.  This function writes a UL for the ISR to tick to the LED.  When the ISR is finished, it 
//    writes the next one.

void tickStatusLEDsequence()
{
  if (flashSequence) return;  // wait until ISR has finished flashing the sequence

  if (ledState == ERROR_STACK) {  // if we just sent a code, make a pause
    flashSequence = SEQ_BETWEEN_CODES;
    ledState = ERROR_STACK_BETWEEN_CODES;
  } else {
    ++whichErrorEntry;
    if (whichErrorEntry >= errorStackIdx) {
      populateErrorStack();
      whichErrorEntry = 0;
    }
    if (errorStackIdx == 0) {
      flashSequence = SEQ_NO_ERROR;
      ledState = OK;
    } else {
      ledState = ERROR_STACK;
      byte errorcode = errorStack[whichErrorEntry];
      unsigned long flashSeq = 0;
      for (int i = 0; i < 8; ++i) {
         flashSeq <<= 4;
         flashSeq |= (errorcode & 0x80) ? ONE_BIT_CODE : ZERO_BIT_CODE;
         errorcode <<= 1; 
      }
      flashSequence = flashSeq;
    }
  }
}

// ISR to update the LED state from a comms register
void updateStatusLEDisr()
{
  pinStatusLED.write((flashSequence & 0x80000000UL)^0x80000000UL);
  flashSequence <<= 1;
}
