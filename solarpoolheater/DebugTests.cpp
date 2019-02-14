#include "DebugTests.h"
#include "Commands.h"
#include "RealTimeClock.h"
#include "SystemStatus.h"

// perform some tests on OutputDestinationEthernet
int runTest1(int variable)
{
          // 0 = process command via parseIncomingInput:
          //   first = missing !, second = no trailing \0, third = too long, fourth = valid status command
        switch (variable) {
          case 0: {
            char test0a[] = "no!"; 
            parseIncomingInput(test0a, sizeof test0a, &Serial);
            char test0b[] = "!no"; 
            parseIncomingInput(test0b, sizeof test0b - 1, &Serial);
            char *junk = (char *)malloc(MAX_COMMAND_LENGTH + 4);
            if (NULL != junk) {
              memset(junk, '!', MAX_COMMAND_LENGTH + 4);
              junk[MAX_COMMAND_LENGTH+3] = '\0';
              parseIncomingInput(junk, MAX_COMMAND_LENGTH + 4, &Serial);
              free(junk);
            }
            char test0d[] = "!s";
            parseIncomingInput(test0d, sizeof test0d, &Serial);
            break;
          }
        } 

}

int runTest2(int variable)
{
 // things to test manually:
//  correct time goes in logging and in datastream
//  change time using command works
// resynch time from rpi works - sets time and timezone correctly, periodic resycnh works correctly

//    DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);

//  print datetime with timezone and without

  DateTime test1(2019, 10, 15, 20, 15, 30);
  DateTime test2(2019, 10, 15, 3, 45, 10);
  DateTime test3(2019, 06, 05, 20, 15, 30);
  
  printDateTime(*serialConsole, test1);
  printDateTime(*serialConsole, test2);
  printDateTime(*serialConsole, test3);
  printDateTimeWithZone(*serialConsole, test1, 0);
  printDateTimeWithZone(*serialConsole, test1, 3600);
  printDateTimeWithZone(*serialConsole, test1, -5400);
  printDateTimeWithZone(*serialConsole, test2, 0);
  printDateTimeWithZone(*serialConsole, test2, -3600*4);
  printDateTimeWithZone(*serialConsole, test3, 0);
  printDateTimeWithZone(*serialConsole, test3, 3600 * 4);

  DateTime retDateTime;
  long retTimeZone;
  bool success;
  DateTime test4(2009, 12, 26, 12, 34, 56);
  success = parseDateTimeWithZone("Dec 26 2009 12:34:56UTC+09:30", retDateTime, retTimeZone);
  uint32_t test4secs = test4.unixtime() - 9.5 * 3600;
  serialConsole->print("test4:");
  printDateTimeWithZone(*serialConsole, retDateTime, retTimeZone);
  serialConsole->print(success);
  serialConsole->print(test4secs); serialConsole->print(": ");
  serialConsole->print(retDateTime.unixtime()); serialConsole->print(", ");
  serialConsole->println(retTimeZone);

  success = parseDateTimeWithZone("Dec 26 2009 12:34:56UTC-01:15", retDateTime, retTimeZone);
  uint32_t test5secs = test4.unixtime() + 1.25 * 3600;
  serialConsole->print("test5:");
  printDateTimeWithZone(*serialConsole, retDateTime, retTimeZone);
  serialConsole->print(success);
  serialConsole->print(test5secs); serialConsole->print(": ");
  serialConsole->print(retDateTime.unixtime()); serialConsole->print(", ");
  serialConsole->println(retTimeZone);

  serialConsole->println("resynchtests");
  DateTime savedTime = currentTimeUTC;
  long savedTimeZone = currentTimeZoneSeconds;
  long mismatchfound = setDateTimeForResynchronisation(savedTime.unixtime(), savedTimeZone);
  serialConsole->println(mismatchfound);
  serialConsole->println(getResynchStatus());    
  serialConsole->println(tickResynchronise());    
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);

  mismatchfound = setDateTimeForResynchronisation(savedTime.unixtime() + 3, savedTimeZone);
  serialConsole->println(mismatchfound);
  serialConsole->println(getResynchStatus());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);
  
  mismatchfound = setDateTimeForResynchronisation(savedTime.unixtime(), savedTimeZone);
  serialConsole->println(mismatchfound);
  serialConsole->println(getResynchStatus());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  serialConsole->println(tickResynchronise());    
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);

  mismatchfound = setDateTimeForResynchronisation(savedTime.unixtime() + 1000, savedTimeZone);
  serialConsole->println(mismatchfound);
  serialConsole->println(getResynchStatus());    
  serialConsole->println(tickResynchronise());    
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);

  mismatchfound = setDateTimeForResynchronisation(currentTimeUTC.unixtime(), savedTimeZone+3600);
  serialConsole->println(mismatchfound);
  serialConsole->println(getResynchStatus());    
  serialConsole->println(tickResynchronise());    
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);

  mismatchfound = setDateTimeForResynchronisation(savedTime.unixtime() + 10, savedTimeZone);
  refreshCurrentTime();
  printDateTimeWithZone(*serialConsole, currentTimeUTC, currentTimeZoneSeconds);
  return 0;
}

int runTest(int testnumber)
{
  switch(testnumber) {
    case 0: {
      return runTest1(testnumber);
    }
    case 1: {
      return runTest2(testnumber);
    }
    default:
      return -1;
  }	

}
