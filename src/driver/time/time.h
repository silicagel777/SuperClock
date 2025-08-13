#pragma once

#include <stdint.h>

class Time {
public:
  Time();
  ~Time();
  uint32_t milliseconds();

private:
  Time(const Time &) = delete;
  void operator=(const Time &) = delete;
};
