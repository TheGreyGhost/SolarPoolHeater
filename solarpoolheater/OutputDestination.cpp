#include "OutputDestination.h"

void OutputDestinationSerial::begin()
{
  Serial.begin(9600);
}

size_t OutputDestinationSerial::write(uint8_t b)
{
  return Serial.write(b);
}

size_t OutputDestinationSerial::write(const uint8_t *buf, size_t size)
{
  return Serial.write(buf, size);
}

OutputDestinationEthernet::OutputDestinationEthernet(EthernetUDP &udpConnection, IPAddress remoteIP, unsigned int remotePort)
 : m_udpConnection(udpConnection), m_remoteIP(remoteIP), m_remotePort(remotePort)
{
}

void OutputDestinationEthernet::begin()
{
  // nothing required since we already supplied an initialised connection
  m_queuedBytes = 0;
}

size_t OutputDestinationEthernet::write(uint8_t b)
{
  return write(&b, 1);
}

size_t OutputDestinationEthernet::write(const uint8_t *buf, size_t size)
{
  const MAX_PACKET_SIZE = 500;
  size_t bytesSent = 0;

// keep sending chunks until we have less than the MAX packet size remaining
  while (m_queuedBytes + (size - bytesSent) >= MAX_PACKET_SIZE) {
    size_t bytesToAdd = MAX_PACKET_SIZE - m_queuedBytes;
    addChunk(buf + bytesSent, bytesToAdd, true);
    bytesSent += bytesToAdd;
  }

// find the last \n
  for (int i = size; i > bytesSent && buf[i-1] != '\n'; --i);
  
  if (i > bytesSent) {  // found one
    size_t bytesToAdd = i - bytesSent;
    addChunk(buf + bytesSent, bytesToAdd, true);
    bytesSent += bytesToAdd;
  }
  if (bytesSent < size) {
    addChunk(buf + bytesSent, bytesToAdd, false);
  }
  return size;
}

// add a chunk to the packet.  Start a new packet if necessary, end the packet if flush is true
size_t OutputDestinationEthernet::addChunk(const uint8_t *buf, size_t size, bool flush)
{
  if (m_queuedBytes == 0) {
    m_udpConnection.beginPacket(m_remoteIP, m_remotePort);
  }
  m_udpConnection.write(buf, size);
  if (flush) {
    m_udpConnection.endPacket();     
    m_queuedBytes = 0;
  } else {
    m_queuedBytes += size;
  }  
  return size;
}


