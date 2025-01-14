#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <util/delay.h>

#include "display.h"
#include "driver/display/display.h"
#include "driver/display/numbers.h"

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
static volatile uint8_t g_buf[Display::c_height][Display::c_width];

struct ColumnPin {
  volatile uint8_t *port;
  uint8_t mask;
  uint8_t maskInv;
};

static const ColumnPin columnPins[] = {
    {&PORTD, (uint8_t)(1 << 7), (uint8_t) ~(1 << 7)}, // C0
    {&PORTC, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C1
    {&PORTC, (uint8_t)(1 << 3), (uint8_t) ~(1 << 3)}, // C2
    {&PORTC, (uint8_t)(1 << 4), (uint8_t) ~(1 << 4)}, // C3
    {&PORTC, (uint8_t)(1 << 5), (uint8_t) ~(1 << 5)}, // C4
    {&PORTC, (uint8_t)(1 << 6), (uint8_t) ~(1 << 6)}, // C5
    {&PORTC, (uint8_t)(1 << 7), (uint8_t) ~(1 << 7)}, // C6
    {&PORTA, (uint8_t)(1 << 6), (uint8_t) ~(1 << 6)}, // C7
    {&PORTA, (uint8_t)(1 << 5), (uint8_t) ~(1 << 5)}, // C8
    {&PORTA, (uint8_t)(1 << 4), (uint8_t) ~(1 << 4)}, // C9
    {&PORTA, (uint8_t)(1 << 3), (uint8_t) ~(1 << 3)}, // C10
    {&PORTA, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C11
    {&PORTA, (uint8_t)(1 << 1), (uint8_t) ~(1 << 1)}, // C12
    {&PORTA, (uint8_t)(1 << 0), (uint8_t) ~(1 << 0)}, // C13
    {&PORTB, (uint8_t)(1 << 0), (uint8_t) ~(1 << 0)}, // C14
    {&PORTB, (uint8_t)(1 << 1), (uint8_t) ~(1 << 1)}, // C15
    {&PORTB, (uint8_t)(1 << 2), (uint8_t) ~(1 << 2)}, // C16
};

static inline void displayInit() {
  // Init outputs
  DDRA |= 0b01111111;
  DDRB |= 0b00010111;
  DDRC |= 0b11111100;
  DDRD |= 0b11111111;

  // Timer 2, fast PWM mode, F_CPU/8
  TCCR2 = (1 << WGM21) | (1 << WGM20) | (0 << CS22) | (1 << CS21) | (0 << CS20);
  // Set output compare (medium brightness)
  OCR2 = 127;
  // Enable the compare interrupt and overflow interrupt
  TIMSK |= (1 << OCIE2) | (1 << TOIE2);
  // Now enable global interrupts
  sei();
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
  uint8_t maskB = (g_buf[1][g_column] > g_pixelBrightnessStep) << 4;  // DOT0
  uint8_t maskD = (g_buf[4][g_column] > g_pixelBrightnessStep) << 0 | // DOT1
                  (g_buf[5][g_column] > g_pixelBrightnessStep) << 1 | // A5
                  (g_buf[4][g_column] > g_pixelBrightnessStep) << 2 | // A4
                  (g_buf[3][g_column] > g_pixelBrightnessStep) << 3 | // A3
                  (g_buf[2][g_column] > g_pixelBrightnessStep) << 4 | // A2
                  (g_buf[1][g_column] > g_pixelBrightnessStep) << 5 | // A1
                  (g_buf[0][g_column] > g_pixelBrightnessStep) << 6;  // A0
  PORTB = (PORTB & ~0b00010000) | maskB;
  PORTD = (PORTD & ~0b01111111) | maskD;
  return maskB | maskD;
}

static inline ColumnPin displayGetColumnPin() {
  return columnPins[g_column];
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

void Display::clear() {
  memset((void *)g_buf, 0, c_height * c_width);
}

void Display::writePixel(uint8_t x, uint8_t y, uint8_t brightness) {
  g_buf[y][x] = brightness;
}

uint8_t Display::readPixel(uint8_t x, uint8_t y) {
  return g_buf[y][x];
}

void Display::writeBmp(
    const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t brightness) {
  for (uint8_t i = 0; i < w; i++) {
    for (uint8_t j = 0; j < h; j++) {
      // TODO: optimize
      int16_t bufx = i + x;
      int16_t bufy = j + y;
      if (bufx >= 0 && bufx < c_width && bufy >= 0 && bufy < c_height) {
        g_buf[bufy][bufx] = bmp[i + j * w] ? brightness : 0;
      }
    }
  }
}

void Display::writeBmpProgmem(
    const uint8_t *bmp, int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t brightness) {
  for (uint8_t i = 0; i < w; i++) {
    for (uint8_t j = 0; j < h; j++) {
      // TODO: optimize
      int16_t bufx = i + x;
      int16_t bufy = j + y;
      if (bufx >= 0 && bufx < c_width && bufy >= 0 && bufy < c_height) {
        g_buf[bufy][bufx] = pgm_read_byte(bmp + i + j * w) ? brightness : 0;
      }
    }
  }
}

void Display::writeNumber(uint8_t n, int16_t x, int16_t y, uint8_t brightness) {
  if (n >= 10) {
    n = 0;
  }
  writeBmpProgmem((const uint8_t *)cp_numbers[n], x, y, c_numberWidth, c_numberHeight, brightness);
}
