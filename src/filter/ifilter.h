#pragma once

#include <stdint.h>

class IFilter {
public:
  virtual void reset(uint16_t initialValue) = 0;
  virtual uint16_t filter(uint16_t input) = 0;

protected:
  ~IFilter() = default;
};
