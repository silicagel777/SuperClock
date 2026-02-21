#pragma once

#include <stdint.h>

class ICalib {
public:
  virtual uint8_t readCalib(int8_t &calib) = 0;
  virtual uint8_t setCalib(int8_t calib) = 0;

protected:
  ~ICalib() = default;
};
