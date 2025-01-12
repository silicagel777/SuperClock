#pragma once

#include <stdint.h>

class ITemp {
public:
  // Result is celsius*100
  virtual uint8_t readTemp(int16_t *temp) = 0;

protected:
  ~ITemp() = default;
};
