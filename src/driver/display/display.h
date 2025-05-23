#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;
  static constexpr uint8_t c_maxX = c_width - 1;
  static constexpr uint8_t c_maxY = c_height - 1;
  static constexpr uint8_t c_centerX = c_width / 2;
  static constexpr uint8_t c_centerY = c_height / 2;
  static constexpr uint8_t c_maxPixelBrightness = 7;
  static constexpr uint8_t c_maxGlobalBrightness = 255;
  enum class Align {
    LEFT,   // align left
    RIGHT,  // align right
    CENTER, // align center
    MIDDLE, // align around middle character (or middle space if character count is even)
  };

  Display();
  void setGlobalBrightness(uint8_t brightness);
  uint8_t getGlobalBrightness();
  void update();
  void clear();
  void writePixel(uint8_t x, uint8_t y, uint8_t brightness = c_maxPixelBrightness);
  uint8_t readPixel(uint8_t x, uint8_t y);
  void writeBmp(const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h,
      uint8_t brightness = c_maxPixelBrightness);
  void writeBmpProgMem(const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h,
      uint8_t brightness = c_maxPixelBrightness);
  void writeChar(char c, int16_t x, int16_t y, uint8_t brightness = c_maxPixelBrightness);
  void writeString(const char *s, int16_t x, int16_t y, Align align = Align::LEFT,
      uint8_t brightness = c_maxPixelBrightness);
  void writeClockNums(
      uint8_t leftNum, char sep, uint8_t rightNum, uint8_t brightness = c_maxPixelBrightness);
  void writeYearNum(uint16_t year, uint8_t brightness = c_maxPixelBrightness);
  void writeBottomLine(uint8_t start, uint8_t end, uint8_t brightness = c_maxPixelBrightness);

private:
  struct CharData {
    uint8_t w;
    uint8_t h;
    const uint8_t *bmpProgMem;
  };

  Display(const Display &) = delete;
  void operator=(const Display &) = delete;
  CharData getCharData(char c);
  uint16_t getStringWidth(const char *s);
  uint16_t getStringMiddleOffset(const char *s);
};
