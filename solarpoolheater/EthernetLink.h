#ifndef ETHERNETLINK_H   
#define ETHERNETLINK_H
#include <Arduino.h>

enum EthernetStatus {ES_ETHERNET_NO_HARDWARE = 0, ES_LINK_OFF = 1, ES_NO_SOCKETS = 2, ES_OK = 3};
extern const char* ethernetStatusText[4];

extern EthernetStatus ethernetStatus;

void setupEthernet();
void tickEthernet();
void sendEthernetMessage(const byte msg[], int messagelength);

#endif
