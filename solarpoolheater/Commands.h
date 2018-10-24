#ifndef COMMANDS_H   
#define COMMANDS_H  
#include "GlobalDefines.h"
#include <Arduino.h>

// prepare for receiving/executing commands
void setupCommands();

// start executing the given command
void executeCommand(char command[]);

//call at frequent intervals (eg 100 ms) to check for new commands or continue the processing of any command currently in progress
void tickCommands();


#endif
