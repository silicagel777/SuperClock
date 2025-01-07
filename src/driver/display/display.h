#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;

  Display();

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;

};
