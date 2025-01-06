#pragma once

#include <stdint.h>

class ITone {
public:
  virtual void run(uint16_t freq) = 0;
  virtual void stop() = 0;

protected:
  ~ITone() = default;
};
