#ifndef COMMANDS_H   
#define COMMANDS_H  
#include <Arduino.h>

// prepare for receiving/executing commands
void setupCommands();

// start executing the given command
void executeCommand(char command[]);

//call at frequent intervals (eg 100 ms) to check for new commands or continue the processing of any command currently in progress
void tickCommands();

// process a command input; respond to 'reply'
// command input must start with the COMMAND_START_CHAR and end with '\0'. bufferlen can be longer than the command.
void parseIncomingInput(char command[], int bufferlen, Print *replyConsole);

const int MAX_COMMAND_LENGTH = 30;

#endif
