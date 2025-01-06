#pragma once

#include <stdint.h>
#include <system.h>

class Time {
public:
  Time() {}

  inline uint32_t milliseconds() {
    return systemMilliseconds();
  }

private:
  Time(const Time &) = delete;
  void operator=(const Time &) = delete;
};
