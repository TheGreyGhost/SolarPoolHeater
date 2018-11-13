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

// buffers for receiving and sending data
const int UDP_PACKET_CHUNK_SIZE = 64;           // greater than COMMAND_BUFFER_SIZE
char packetBufferChunk[UDP_PACKET_CHUNK_SIZE];  // buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP udpConnectionTerminal;
EthernetUDP udpConnectionDatastream;

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
    int success1 = udpConnectionTerminal.begin(PORT_LOCAL_TERMINAL);  
    int success2 = udpConnectionDatastream.begin(PORT_LOCAL_DATASTREAM);
    
    if (success1 != 1 || success2 != 1) {
      ethernetStatus = ES_NO_SOCKETS;
      outputDestinationTerminal = new OutputDestinationEthernetNULL(udpConnectionTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
    } else {  
      ethernetStatus = ES_OK;
      Serial.print("UDP server is at ");
      Serial.print(Ethernet.localIP());
      Serial.print(": terminal port ");
      Serial.print(PORT_LOCAL_TERMINAL);
      Serial.print(" and datastream port ");
      Serial.println(PORT_LOCAL_DATASTREAM);
      outputDestinationTerminal = new OutputDestinationEthernet(udpConnectionTerminal, IP_REMOTE, PORT_REMOTE_TERMINAL);
    }  
  }
}

TODO: test my changes that 
1) flip console between serial and ethernet
2) assign a serialConsole to console

void tickEthernet() {
  // if there's data available, read a packet
  // if it's on the terminal port, parse it as a command
  // otherwise, echo the packet to the console (for now - debugging)
  int packetSize = udpConnectionTerminal.parsePacket();
  if (packetSize) {
    int numofchars = udpConnectionTerminal.read(packetBufferChunk, UDP_PACKET_CHUNK_SIZE-1);  // discard the rest (when next parsePacket is called)
    if (numofchars >= 0 && numofchars < UDP_PACKET_CHUNK_SIZE) {
      packetBufferChunk[numofchars] = '\0';
      console = outputDestinationTerminal;
      parseIncomingInput(packetBufferChunk, numofchars + 1, outputDestinationTerminal);  // todo later change to Ethernet terminal for reply
    }  
  }

  packetSize = udpConnectionDatastream.parsePacket();
  if (packetSize) {
    IPAddress remoteIP = udpConnectionDatastream.remoteIP();
    Serial.print("Port ");
    Serial.print(udpConnectionDatastream.localPort());
    Serial.print(" received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    for (int i=0; i < 4; i++) {
      Serial.print(remoteIP[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(":");
    Serial.println(udpConnectionDatastream.remotePort());
    Serial.println("Contents:");

    // read the packet into packetBufffer
    int numofchars;
    bool readmore = true;
    do {
      numofchars = udpConnectionDatastream.read(packetBufferChunk, UDP_PACKET_CHUNK_SIZE);
      if (numofchars < 0) {
        readmore = false;
      } else {
        Serial.write(packetBufferChunk, numofchars);
      }
    } while (readmore);
    Serial.println();
  }
}

void sendEthernetTerminalMessage(const byte msg[], int messagelength)
{
    udpConnectionTerminal.beginPacket(IP_REMOTE, PORT_REMOTE_TERMINAL);
    udpConnectionTerminal.write(msg, messagelength);
    udpConnectionTerminal.endPacket();
}

// start a DataStream message; provides an EthernetUDP to use
EthernetUDP &prepareEthernetDatastreamMessage()
{
  udpConnectionDataStream.beginPacket(IP_REMOTE, PORT_REMOTE_DATASTREAM);
  return udpConnectionDataStream;
}

