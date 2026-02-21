#pragma once

#include <stdint.h>

class Display;

class Panic {
public:
  enum class Error : uint8_t {
    ERR_UNKNOWN = 0,
    ERR_SCHED_FULL = 1,
    ERR_SCHED_BUG = 2,
  };

  Panic(Display &display);
  ~Panic();
  void panic(Error error);

private:
  Panic(const Panic &) = delete;
  Display &m_display;
};
