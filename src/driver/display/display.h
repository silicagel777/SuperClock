#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;

  Display();
  void setDot(uint8_t x, uint8_t y, uint8_t value);
  uint8_t getDot(uint8_t x, uint8_t y);
  

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;

};
