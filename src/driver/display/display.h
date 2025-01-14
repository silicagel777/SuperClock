#pragma once

#include <stdint.h>

class Display {
public:
  static constexpr uint8_t c_width = 17;
  static constexpr uint8_t c_height = 6;
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
  void writeBmpProgmem(const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h,
      uint8_t brightness = c_maxPixelBrightness);
  void writeChar(char c, int16_t x, int16_t y, uint8_t brightness = c_maxPixelBrightness);
  void writeString(char *s, int16_t x, int16_t y, Align align = Align::LEFT,
      uint8_t brightness = c_maxPixelBrightness);

private:
  struct CharData {
    uint8_t w;
    uint8_t h;
    const uint8_t *data;
  };

  Display(const Display &) = delete;
  void operator=(const Display &) = delete;
  CharData getCharData(char c);
  uint16_t getStringWidth(char *s);
  uint16_t getStringMiddleOffset(char *s);
};
