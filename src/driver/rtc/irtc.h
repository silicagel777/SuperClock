#pragma once

#include <stdint.h>

class IRtc {
public:
  struct RtcTime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t week;
    uint8_t month;
    uint16_t year;
  };

  virtual uint8_t readTime(RtcTime *time) = 0;

protected:
  ~IRtc() = default;
};
