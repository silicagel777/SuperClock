#pragma once

#include <filter/ifilter.h>
#include <stdint.h>

class DummyFilter : public IFilter {
public:
  virtual void reset(uint16_t initialValue) {};
  virtual uint16_t filter(uint16_t input) {
    return input;
  }
};
