#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;
  static constexpr uint8_t c_maxPixelBrigtness = 7;
  static constexpr uint8_t c_maxGlobalBrightness = 255;

  Display();
  void setGlobalBrigntess(uint8_t brightness);
  uint8_t getGlobalBrightness();
  void writePixel(uint8_t x, uint8_t y, uint8_t brigtness);
  uint8_t readPixel(uint8_t x, uint8_t y);
  void writeBmp(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp);

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;

};
