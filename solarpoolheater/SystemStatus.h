#ifndef DEBUG_H   
#define DEBUG_H  
#include <Arduino.h>
extern byte assertFailureCode;

#define ASSERT_INVALID_SWITCH 1
#define ASSERT_INDEX_OUT_OF_BOUNDS 2

extern Print *console;
extern Stream *consoleInput;

void setupSystemStatus();

void tickSystemStatus();

void printDebugInfo(Print &dest);

// assign numbers for each error code
const byte ERRORCODE_PROBE = 16;   // leave space for NUMBER_OF_PROBES, ie 16 = probe 0, 17 = probe 1, etc
const byte ERRORCODE_DATALOG = 32; // leave space for error codes
const byte ERRORCODE_ASSERT = 48; // leave space for error codes
const byte ERRORCODE_RTC = 64; // only takes up one slot
const byte ERRORCODE_SOLAR_SENSOR = 65; // only takes up one slot

// no error = steady on off     .#.#.#.#  
// error patterns are msb first.  zero = short, one = long.  eg 0 0 1 1 is #... #... ###. ###.

#endif
