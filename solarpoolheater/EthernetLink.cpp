#include "EthernetLink.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "DebugTests.h"
#include "Commands.h"

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
const int UDP_PACKET_CHUNK_SIZE = 64;           // greater than COMMAND_BUFFER_SIZE
char packetBufferChunk[UDP_PACKET_CHUNK_SIZE];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP udpConnection;

OutputDestinationEthernet *outputDestinationTerminal;

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
    int success = udpConnection.begin(PORT_LOCAL_TERMINAL);
    
    if (success != 1) {
      ethernetStatus = ES_NO_SOCKETS;
      outputDestinationTerminal = new OutputDestinationEthernetNULL(udpConnection, IP_REMOTE, PORT_REMOTE_TERMINAL);
    } else {  
      ethernetStatus = ES_OK;
      Serial.print("UDP server is at ");
      Serial.print(Ethernet.localIP());
      Serial.print(":");
      Serial.print(PORT_LOCAL_TERMINAL);
      outputDestinationTerminal = new OutputDestinationEthernet(udpConnection, IP_REMOTE, PORT_REMOTE_TERMINAL);
    }  
  }
}

void tickEthernet() {
  // if there's data available, read a packet
  // if it's on the terminal port, parse it as a command
  // otherwise, echo the packet to the console (for now - debugging)
  int packetSize = udpConnection.parsePacket();
  if (packetSize) {
    if (udpConnection.localPort() == PORT_LOCAL_TERMINAL) {
        int numofchars = udpConnection.read(packetBufferChunk, UDP_PACKET_CHUNK_SIZE-1);  // discard the rest (when next parsePacket is called)
        if (numofchars >= 0 && numofchars < UDP_PACKET_CHUNK_SIZE) {
          packetBufferChunk[numofchars] = '\0';
          parseIncomingInput(packetBufferChunk, numofchars, &Serial);  // todo later change to Ethernet terminal for reply
        }  
    } else {
      IPAddress remoteIP = udpConnection.remoteIP();
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      for (int i=0; i < 4; i++) {
        Serial.print(remoteIP[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.println(udpConnection.remotePort());
      Serial.println("Contents:");

      // read the packet into packetBufffer
      int numofchars;
      bool readmore = true;
      do {
        numofchars = udpConnection.read(packetBufferChunk, UDP_PACKET_CHUNK_SIZE);
        if (numofchars < 0) {
          readmore = false;
        } else {
          Serial.write(packetBufferChunk, numofchars);
        }
      } while (readmore);
      Serial.println();
    }
  }
}

void sendEthernetTerminalMessage(const byte msg[], int messagelength)
{
    udpConnection.beginPacket(IP_REMOTE, PORT_REMOTE_TERMINAL);
    udpConnection.write(msg, messagelength);
    udpConnection.endPacket();
}
