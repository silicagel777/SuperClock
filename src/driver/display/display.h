#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;

  Display();
  void process();

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;

  uint8_t m_col = 0;
  uint8_t m_buf[Display::c_width][Display::c_height] = {
      {0, 0, 0, 0, 0, 0},
      {0, 255, 0, 0, 0, 0},
      {255, 255, 255, 255, 255, 0},
      {0, 0, 0, 0, 0, 0},
      {255, 0, 255, 0, 255, 0},
      {255, 0, 255, 0, 255, 0},
      {255, 255, 255, 255, 255, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 255, 0, 0, 255, 0},
      {0, 0, 0, 0, 0, 0},
      {255, 0, 255, 255, 255, 0},
      {255, 0, 255, 0, 255, 0},
      {255, 255, 255, 0, 255, 0},
      {0, 0, 0, 0, 0, 0},
      {255, 0, 255, 0, 255, 0},
      {255, 0, 255, 0, 255, 255},
      {255, 255, 255, 255, 255, 0},
  };
};
