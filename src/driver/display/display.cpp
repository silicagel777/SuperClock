#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "driver/display/display.h"
#include "driver/display/font/font.h"
#include "driver/mem/prog.h"

/******************************************************************************
  Ugly stuff ahead! The idea is to drive dot matrix in software by switching
  displayed column on timer overflow interrupt.

  Global brightness is set up by disabling display on compare interrupt
  of the same timer. If two interrupts become too close, the compare interrupt
  is disabled and the corresponding logic is performed on overflow.

  Per-pixel brightness is set up by treating each display refresh as pixel
  brightness step and enabling or disabling pixels by comparing their values
  to step number.
******************************************************************************/

static constexpr uint8_t c_compareLowest = 25;
static constexpr uint8_t c_compareHighest = 230;

static volatile uint8_t g_column = 0;
static volatile uint8_t g_pixelBrightnessStep = 0;
static volatile uint8_t g_buf[2][Display::c_height * Display::c_width];
static volatile uint8_t *g_fgBuf = g_buf[0];
static volatile uint8_t *g_bgBuf = g_buf[1];

struct ColumnPin {
  volatile uint8_t *port;
  uint8_t mask;
  uint8_t maskInv;
};

static const ColumnPin c_columnPins[] = {
    {&PORTC, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C0
    {&PORTC, (uint8_t)(1 << 3), (uint8_t) ~(1 << 3)}, // C1
    {&PORTC, (uint8_t)(1 << 4), (uint8_t) ~(1 << 4)}, // C2
    {&PORTC, (uint8_t)(1 << 5), (uint8_t) ~(1 << 5)}, // C3
    {&PORTC, (uint8_t)(1 << 6), (uint8_t) ~(1 << 6)}, // C4
    {&PORTC, (uint8_t)(1 << 7), (uint8_t) ~(1 << 7)}, // C5
    {&PORTA, (uint8_t)(1 << 6), (uint8_t) ~(1 << 6)}, // C6
    {&PORTA, (uint8_t)(1 << 5), (uint8_t) ~(1 << 5)}, // C7
    {&PORTA, (uint8_t)(1 << 4), (uint8_t) ~(1 << 4)}, // C8
    {&PORTA, (uint8_t)(1 << 3), (uint8_t) ~(1 << 3)}, // C9
    {&PORTA, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C10
    {&PORTA, (uint8_t)(1 << 1), (uint8_t) ~(1 << 1)}, // C11
    {&PORTA, (uint8_t)(1 << 0), (uint8_t) ~(1 << 0)}, // C12
    {&PORTB, (uint8_t)(1 << 0), (uint8_t) ~(1 << 0)}, // C13
    {&PORTB, (uint8_t)(1 << 1), (uint8_t) ~(1 << 1)}, // C14
    {&PORTB, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C15
    {&PORTB, (uint8_t)(1 << 4), (uint8_t) ~(1 << 4)}, // C16
};
static constexpr uint8_t c_pinMaskA = 0b01111111;
static constexpr uint8_t c_pinMaskB = 0b00010111;
static constexpr uint8_t c_pinMaskC = 0b11111100;
static constexpr uint8_t c_pinMaskD = 0b11111111;

static inline void displayInit() {
  // Init outputs
  DDRA |= c_pinMaskA;
  DDRB |= c_pinMaskB;
  DDRC |= c_pinMaskC;
  DDRD |= c_pinMaskD;

  // Timer 2, fast PWM mode, F_CPU/8
  TCCR2 = (1 << WGM21) | (1 << WGM20) | (0 << CS22) | (1 << CS21) | (0 << CS20);
  // Set output compare (medium brightness)
  OCR2 = 127;
  // Enable the compare interrupt and overflow interrupt
  TIMSK |= (1 << OCIE2) | (1 << TOIE2);
}

static inline void displayDeinit() {
  // De-init outputs
  PORTA &= ~c_pinMaskA;
  PORTB &= ~c_pinMaskB;
  PORTC &= ~c_pinMaskC;
  PORTD &= ~c_pinMaskD;
  DDRA &= ~c_pinMaskA;
  DDRB &= ~c_pinMaskB;
  DDRC &= ~c_pinMaskC;
  DDRD &= ~c_pinMaskD;

  // De-init timer
  TCCR2 = 0;
  OCR2 = 0;
  TIMSK &= ~(1 << OCIE2) | ~(1 << TOIE2);
}

static inline void displaySetBrightness(uint8_t brightness) {
  OCR2 = brightness;
  if (OCR2 < c_compareLowest || OCR2 > c_compareHighest) {
    // Compare and overflow interrupts are too close
    // Let's disable compare interrupt and do everything on overflow
    TIMSK &= ~(1 << OCIE2);
  } else {
    // Enable compare interrupt
    TIMSK |= (1 << OCIE2);
  }
}

static inline uint8_t displayGetBrightness() {
  return OCR2;
}

static inline bool displayEnableRows() {
  const uint16_t offset = g_column * Display::c_height;
  uint8_t maskD = (g_fgBuf[offset + 4] > g_pixelBrightnessStep) << 0 | // DOT1
                  (g_fgBuf[offset + 1] > g_pixelBrightnessStep) << 1 | // DOT0
                  (g_fgBuf[offset + 5] > g_pixelBrightnessStep) << 2 | // A5
                  (g_fgBuf[offset + 4] > g_pixelBrightnessStep) << 3 | // A4
                  (g_fgBuf[offset + 3] > g_pixelBrightnessStep) << 4 | // A3
                  (g_fgBuf[offset + 2] > g_pixelBrightnessStep) << 5 | // A2
                  (g_fgBuf[offset + 1] > g_pixelBrightnessStep) << 6 | // A1
                  (g_fgBuf[offset + 0] > g_pixelBrightnessStep) << 7;  // A0
  PORTD = maskD;
  return maskD;
}

static inline ColumnPin displayGetColumnPin() {
  return c_columnPins[g_column];
}

static inline void displayEnableColumn(const ColumnPin &columnPin) {
  *columnPin.port |= columnPin.mask;
}

static inline void displayDisableColumn(const ColumnPin &columnPin) {
  *columnPin.port &= columnPin.maskInv;
}

static inline void displayNextPixelBrightnessStep() {
  g_pixelBrightnessStep++;
  if (g_pixelBrightnessStep >= Display::c_maxPixelBrightness) {
    g_pixelBrightnessStep = 0;
  }
}

static inline void displayNextColumn() {
  g_column++;
  if (g_column >= Display::c_width) {
    g_column = 0;
    displayNextPixelBrightnessStep();
  }
}

ISR(TIMER2_OVF_vect) {
  if (OCR2 > c_compareHighest) {
    // Highest possible brightness
    // Disable column immediately before enabling the next one
    displayDisableColumn(displayGetColumnPin());
  }

  displayNextColumn();
  ColumnPin columnPin = displayGetColumnPin();
  if (displayEnableRows()) {
    // Enable column only if there is something to display: this counteracts capacitance-related
    // ghosting for empty columns. The same issue for non-empty columns is counteracted by keeping
    // rows active even when column is already disabled.
    displayEnableColumn(columnPin);
  }

  if (OCR2 < c_compareLowest) {
    // Lowest possible brightness
    // Disable column immediately after enabling it
    displayDisableColumn(columnPin);
  }
}

ISR(TIMER2_COMP_vect) {
  displayDisableColumn(displayGetColumnPin());
}

/******************************************************************************
  Slighly less ugly stuff ahead: C++ interface part
******************************************************************************/

Display::Display() {
  displayInit();
}

void Display::setGlobalBrightness(uint8_t brightness) {
  displaySetBrightness(brightness);
}

uint8_t Display::getGlobalBrightness() {
  return displayGetBrightness();
}

void Display::update() {
  auto tmp = g_fgBuf;
  g_fgBuf = g_bgBuf;
  g_bgBuf = tmp;
  memcpy((void *)g_bgBuf, (void *)g_fgBuf, c_height * c_width);
}

void Display::clear() {
  memset((void *)g_bgBuf, 0, c_height * c_width);
}

void Display::writePixel(uint8_t x, uint8_t y, uint8_t brightness) {
  if (x < c_width && y < c_height) {
    g_bgBuf[x * c_height + y] = brightness;
  }
}

uint8_t Display::readPixel(uint8_t x, uint8_t y) {
  return g_bgBuf[x * c_height + y];
}

void Display::writeBmp(
    const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t brightness) {
  for (uint8_t i = 0; i < w; i++) {
    int16_t bufx = i + x;
    if (bufx >= 0 && bufx < c_width) {
      for (uint8_t j = 0; j < h; j++) {
        int16_t bufy = j + y;
        if (bufy >= 0 && bufy < c_height) {
          uint16_t bitOffset = i + j * w;
          bool bmpPixel = (bmp[bitOffset / 8] >> (bitOffset % 8)) & 1;
          if (bmpPixel) {
            g_bgBuf[bufx * c_height + bufy] = brightness;
          }
        }
      }
    }
  }
}

void Display::writeBmpProgMem(
    const uint8_t *bmpProgMem, int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t brightness) {
  for (uint8_t i = 0; i < w; i++) {
    int16_t bufx = i + x;
    if (bufx >= 0 && bufx < c_width) {
      for (uint8_t j = 0; j < h; j++) {
        int16_t bufy = j + y;
        if (bufy >= 0 && bufy < c_height) {
          uint16_t bitOffset = i + j * w;
          bool bmpPixel = (ProgMem::readU8(bmpProgMem + bitOffset / 8) >> (bitOffset % 8)) & 1;
          if (bmpPixel) {
            g_bgBuf[bufx * c_height + bufy] = brightness;
          }
        }
      }
    }
  }
}

void Display::writeChar(char c, int16_t x, int16_t y, uint8_t brightness) {
  auto charData = getCharData(c);
  writeBmpProgMem(charData.bmpProgMem, x, y, charData.w, charData.h, brightness);
}

void Display::writeString(const char *s, int16_t x, int16_t y, Align align, uint8_t brightness) {
  switch (align) {
  case Align::LEFT:
    break;
  case Align::RIGHT:
    x -= getStringWidth(s);
    break;
  case Align::CENTER:
    x -= getStringWidth(s) / 2;
    break;
  case Align::MIDDLE:
    x -= getStringMiddleOffset(s);
    break;
  }

  while (char c = *s++) {
    auto charData = getCharData(c);
    writeBmpProgMem(charData.bmpProgMem, x, y, charData.w, charData.h, brightness);
    x += charData.w + 1;
  }
}

void Display::writeClockNums(uint8_t leftNum, char sep, uint8_t rightNum, uint8_t brightness) {
  char s[] = {
      (char)('0' + leftNum / 10),
      (char)('0' + leftNum % 10),
      sep,
      (char)('0' + rightNum / 10),
      (char)('0' + rightNum % 10),
      '\0',
  };
  writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE, brightness);
}

void Display::writeYearNum(uint16_t year, uint8_t brightness) {
  char s[] = {
      (char)('0' + year / 1000 % 10),
      (char)('0' + year / 100 % 10),
      (char)('0' + year / 10 % 10),
      (char)('0' + year % 10),
      '\0',
  };
  writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE, brightness);
}

void Display::writeBottomLine(uint8_t start, uint8_t end, uint8_t brightness) {
  start = start >= c_width ? c_width - 1 : start;
  end = end >= c_width ? c_width - 1 : end;
  for (uint8_t i = start; i <= end; i++) {
    writePixel(i, Display::c_height - 1, brightness);
  }
}

Display::CharData Display::getCharData(char c) {
  uint8_t n = (uint8_t)c;
  if (n >= c_fontFirstChar && n <= c_fontLastChar) {
    const uint16_t charOffset = ProgMem::readU16(cp_fontCharOffset + (n - c_fontFirstChar));
    const uint8_t *charData = cp_fontCharData + charOffset;
    const uint8_t w = ProgMem::readU8(charData + 0);
    const uint8_t h = ProgMem::readU8(charData + 1);
    const uint8_t *data = charData + 2;
    return {w, h, data};
  } else {
    return {0, 0, nullptr};
  }
}

uint16_t Display::getStringWidth(const char *s) {
  uint16_t w = 0;
  while (char c = *s++) {
    w += getCharData(c).w + 1;
  }
  w--;
  return w;
}

uint16_t Display::getStringMiddleOffset(const char *s) {
  uint16_t w = 0;
  auto len = strlen(s);
  auto middle = len / 2;
  for (size_t i = 0; i < middle; i++) {
    w += getCharData(s[i]).w + 1;
  }
  if (len % 2) {
    w += getCharData(s[middle]).w / 2;
  } else {
    w--;
  }
  return w;
}
