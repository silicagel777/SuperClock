#pragma once

#include <stdint.h>

class Time {
public:
  Time();

  uint16_t milliseconds();

private:
  Time(const Time &) = delete;
  void operator=(const Time &) = delete;
};
