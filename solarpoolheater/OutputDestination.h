#ifndef OUTPUT_DESTINATION_H
#define OUTPUT_DESTINATION_H
#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

class OutputDestinationSerial : public Print {
public:
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};


// runs as a terminal : will wait until \n is received before sending packet; or until packet size reaches 500 bytes
class OutputDestinationEthernet : public Print {
public:
  OutputDestinationEthernet(EthernetUDP &udpConnection, IPAddress remoteIP, unsigned int remotePort);
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;

private:
  size_t addChunk(const uint8_t *buf, size_t size, bool flush);

  EthernetUDP &m_udpConnection;
  unsigned int m_remotePort;
  IPAddress m_remoteIP;
  int m_queuedBytes;
  boolean m_textMode; 
};

// will wait until \n is received before sending packet; or until packet size reaches 500 bytes
class OutputDestinationEthernetNULL : public OutputDestinationEthernet {
public:
  OutputDestinationEthernetNULL(EthernetUDP &udpConnection, IPAddress remoteIP, unsigned int remotePort);
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};


#endif
