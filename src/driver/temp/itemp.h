#pragma once

#include <stdint.h>

class ITemp {
public:
  struct TempValue {
    int8_t integer;
    uint8_t fractional; // 1/100
  };

  virtual uint8_t readTemp(TempValue &temp) = 0;

protected:
  ~ITemp() = default;
};
