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
  void clear();
  void writePixel(uint8_t x, uint8_t y, uint8_t brigtness = c_maxPixelBrigtness);
  uint8_t readPixel(uint8_t x, uint8_t y);
  void writeBmp(const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h,
      uint8_t brightness = c_maxPixelBrigtness);
  void writeBmpProgmem(const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h,
      uint8_t brightness = c_maxPixelBrigtness);
  void writeNumber(uint8_t n, int16_t x, int16_t y, uint8_t brightness = c_maxPixelBrigtness);

private:
  Display(const Display &) = delete;
  void operator=(const Display &) = delete;
};
