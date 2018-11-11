#include "DebugTests.h"
#include "Commands.h"

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

int runTest(int testnumber)
{
  switch(testnumber) {
    case 0: {
      return runTest1(testnumber);
    }
    default:
      return -1;
  }	

}
