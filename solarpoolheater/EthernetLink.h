#ifndef ETHERNETLINK_H   
#define ETHERNETLINK_H
#include <Arduino.h>
#include <EthernetUdp.h>
#include "OutputDestination.h"

enum EthernetStatus {ES_ETHERNET_NO_HARDWARE = 0, ES_LINK_OFF = 1, ES_NO_SOCKETS = 2, ES_OK = 3};
extern const char* ethernetStatusText[4];

extern EthernetStatus ethernetStatus;

void setupEthernet();
void tickEthernet();
void sendEthernetTerminalMessage(const byte msg[], int messagelength);

// start a DataStream message; provides an EthernetUDP to use
// true for success
bool prepareEthernetDatastreamMessage(EthernetUDP * &connection);

// retrieve an OutputDestination that will send to the ethernet (for terminal comms)
extern OutputDestinationEthernet *outputDestinationTerminal;

#endif
