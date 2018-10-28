#ifndef OUTPUT_DESTINATION_H
#define OUTPUT_DESTINATION_H
#include <Arduino.h>

class OutputDestinationSerial : public Print {
public:
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};


// will wait until \n is received before sending packet; or until packet size reaches 500 bytes
class OutputDestinationEthernet : public Print {
public:
  OutputDestinationEthernet(EthernetUDP &udpConnection, IPAddress remoteIP, unsigned int remotePort);
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
private:
  EthernetUDP &m_udpConnection;
  unsigned int m_remotePort;
  IPAddress m_remoteIP;
  int m_queuedBytes;
};

#endif
