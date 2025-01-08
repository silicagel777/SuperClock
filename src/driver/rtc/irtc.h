#pragma once

#include <stdint.h>

class IRtc {
public:
  struct RtcTime {
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t date;
    uint8_t mon;
    uint8_t day;
    uint16_t yr;
  };

  virtual uint8_t readTime(RtcTime *time) = 0;

protected:
  ~IRtc() = default;
};
