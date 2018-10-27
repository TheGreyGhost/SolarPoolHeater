#include "EthernetLink.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

const char* ethernetStatusText[4] = {"Ethernet shield not found", "Ethernet cable is not connected", "No sockets available", "OK"};
EthernetStatus ethernetStatus;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte MAC_LOCAL[] = {
  0x31, 0x4E, 0xF8, 0xFB, 0x21, 0xDC
};
IPAddress IP_LOCAL(192, 168, 2, 35);
IPAddress IP_REMOTE(192, 168, 2, 34);

unsigned int PORT_LOCAL = 53201;      // local port to listen on
unsigned int PORT_REMOTE = 53201;      // remote port to send to

// buffers for receiving and sending data
const int UDP_PACKET_CHUNK_SIZE = 64;
char packetBufferChunk[UDP_PACKET_CHUNK_SIZE];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setupEthernet() {
  // Open serial communications and wait for port to open:

  // start the Ethernet connection and the server:
  Ethernet.begin(MAC_LOCAL, IP_LOCAL);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    ethernetStatus = ES_ETHERNET_NO_HARDWARE; 
  } else if (Ethernet.linkStatus() == LinkOFF) {
    ethernetStatus = ES_LINK_OFF;
  } else {
    // start UDP
    int success = Udp.begin(PORT_LOCAL);
    if (success != 1) {
      ethernetStatus = ES_NO_SOCKETS;
    } else {  
      ethernetStatus = ES_OK;
      Serial.print("UDP server is at ");
      Serial.print(Ethernet.localIP());
      Serial.print(":");
      Serial.println(PORT_LOCAL);
    }  
  }
}

void tickEthernet() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    Serial.println("Contents:");

    // read the packet into packetBufffer
    int numofchars;
    bool readmore = true;
    do {
      numofchars = Udp.read(packetBufferChunk, UDP_PACKET_CHUNK_SIZE);
      if (numofchars < 0) {
        readmore = false;
      } else {
        Serial.write(packetBufferChunk, numofchars);
      }
    } while (readmore);
    Serial.println();
  }
}

void sendEthernetMessage(const byte msg[], int messagelength)
{
    Udp.beginPacket(IP_REMOTE, PORT_REMOTE);
    Udp.write(msg, messagelength);
    Udp.endPacket();
}
