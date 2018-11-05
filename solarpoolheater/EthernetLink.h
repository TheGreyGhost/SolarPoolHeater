#ifndef ETHERNETLINK_H   
#define ETHERNETLINK_H
#include <Arduino.h>
#include "OutputDestination.h"

enum EthernetStatus {ES_ETHERNET_NO_HARDWARE = 0, ES_LINK_OFF = 1, ES_NO_SOCKETS = 2, ES_OK = 3};
extern const char* ethernetStatusText[4];

extern EthernetStatus ethernetStatus;

void setupEthernet();
void tickEthernet();
void sendEthernetTerminalMessage(const byte msg[], int messagelength);

// retrieve an OutputDestination that will send to the ethernet (for terminal comms)
extern OutputDestinationEthernet *outputDestinationTerminal;
extern 

#endif
