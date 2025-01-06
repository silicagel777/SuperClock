#pragma once

#include <stdint.h>

class Display {
public:
  Display();
  void process();

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;

  uint8_t m_col = 0;
};
