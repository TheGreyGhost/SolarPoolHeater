#include "EthernetLink.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "DebugTests.h"

const char* ethernetStatusText[4] = {"Ethernet shield not found", "Ethernet cable is not connected", "No sockets available", "OK"};
EthernetStatus ethernetStatus;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte MAC_LOCAL[] = {
  0x31, 0x4E, 0xF8, 0xFB, 0x21, 0xDC
};
IPAddress IP_LOCAL(192, 168, 2, 35);
IPAddress IP_REMOTE(192, 168, 2, 34);

unsigned int PORT_LOCAL_TERMINAL = 53201;      // local port to listen on
unsigned int PORT_REMOTE_TERMINAL = 53201;      // remote port to send to

unsigned int PORT_LOCAL_DATASTREAM = 53202;      // local datastream port to listen on 
unsigned int PORT_REMOTE_DATASTREAM = 53202;      // remote port to send datastream to

// buffers for receiving and sending data
const int UDP_PACKET_CHUNK_SIZE = 64;
char packetBufferChunk[UDP_PACKET_CHUNK_SIZE];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP UdpTerminal;
EthernetUDP UdpDatastream;

OutputDestinationEthernet *outputDestinationTerminal;
OutputDestinationEthernet *outputDestinationDatastream;

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
    int success = UdpTerminal.begin(PORT_LOCAL_TERMINAL);
    if (success == 1) {
      success = UdpDatastream.begin(PORT_LOCAL_DATASTREAM);
    }
    
    if (success != 1) {
      ethernetStatus = ES_NO_SOCKETS;
      outputDestinationTerminal = new OutputDestinationEthernetNULL(UdpTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
      outputDestinationDatastream = new OutputDestinationEthernetNULL(UdpTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
    } else {  
      ethernetStatus = ES_OK;
      Serial.print("UDP server is at ");
      Serial.print(Ethernet.localIP());
      Serial.print(":");
      Serial.print(PORT_LOCAL_TERMINAL);
      Serial.print(" and :");
      Serial.println(PORT_LOCAL_DATASTREAM);
      outputDestinationTerminal = new OutputDestinationEthernet(UdpTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
      outputDestinationDatastream = new OutputDestinationEthernet(UdpTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
    }  
  }
}

void tickEthernet() {
  // if there's data available, read a packet
  int packetSize = UdpTerminal.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = UdpTerminal.remoteIP();
    for (int i=0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(UdpTerminal.remotePort());
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

void sendEthernetMessage(EthernetDestination destination, const byte msg[], int messagelength)
{
    Udp.beginPacket(IP_REMOTE, destination == ED_TERMINAL ? PORT_REMOTE_TERMINAL : PORT_REMOTE_DATASTREAM);
    Udp.write(msg, messagelength);
    Udp.endPacket();
}
