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
