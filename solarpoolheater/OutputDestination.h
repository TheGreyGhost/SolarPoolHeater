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

#endif
