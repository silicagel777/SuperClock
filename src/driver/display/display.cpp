#include <avr/interrupt.h>
#include <avr/io.h>

#include "display.h"
#include "driver/display/display.h"

static constexpr uint8_t c_dot = 1;
static volatile uint8_t g_col = 0;
static volatile uint8_t g_buf[Display::c_width][Display::c_height] = {
    {0, 0, 0, 0, 0, 0},
    {0, c_dot, 0, 0, 0, 0},
    {c_dot, c_dot, c_dot, c_dot, c_dot, 0},
    {0, 0, 0, 0, 0, 0},
    {c_dot, 0, c_dot, 0, c_dot, 0},
    {c_dot, 0, c_dot, 0, c_dot, 0},
    {c_dot, c_dot, c_dot, c_dot, c_dot, 0},
    {0, 0, 0, 0, 0, 0},
    {0, c_dot, 0, 0, c_dot, 0},
    {0, 0, 0, 0, 0, 0},
    {c_dot, 0, c_dot, c_dot, c_dot, 0},
    {c_dot, 0, c_dot, 0, c_dot, 0},
    {c_dot, c_dot, c_dot, 0, c_dot, 0},
    {0, 0, 0, 0, 0, 0},
    {c_dot, 0, c_dot, 0, c_dot, 0},
    {c_dot, 0, c_dot, 0, c_dot, c_dot},
    {c_dot, c_dot, c_dot, c_dot, c_dot, 0},
};

static inline void display_init() {
  // Init outputs
  DDRA |= 0b01111111;
  DDRB |= 0b00010111;
  DDRC |= 0b11111100;
  DDRD |= 0b11111111;

  // Timer 2, fast PWM mode, F_CPU/32
  TCCR2 = (1 << WGM21) | (1 << WGM20) | (1 << CS21) | (1 << CS20);
  // Set output compare
  OCR2 = 1;
  // Enable the compare match interrupt and overflow interrupt
  TIMSK |= (1 << OCIE2) | (1 << TOIE2);
  // Now enable global interrupts
  sei();
}

static inline void display_set_brigtness(uint8_t brightness) {
  brightness = 255 - brightness;
  if (brightness == 0) {
    // Prevents two interrupts from clashing
    brightness = 1;
  }
  OCR2 = brightness;
}

static inline void display_off() {
  // Reset outputs
  PORTA &= ~0b01111111;
  PORTB &= ~0b00010111;
  PORTC &= ~0b11111100;
  PORTD &= ~0b11111111;
}

static inline void display_step() {
  // Write rows
  PORTB |= (g_buf[g_col][1] ? 1 : 0) << 4;  // DOT0
  PORTD |= (g_buf[g_col][4] ? 1 : 0) << 0 | // DOT1
           (g_buf[g_col][5] ? 1 : 0) << 1 | // A5
           (g_buf[g_col][4] ? 1 : 0) << 2 | // A4
           (g_buf[g_col][3] ? 1 : 0) << 3 | // A3
           (g_buf[g_col][2] ? 1 : 0) << 4 | // A2
           (g_buf[g_col][1] ? 1 : 0) << 5 | // A1
           (g_buf[g_col][0] ? 1 : 0) << 6;  // A0

  // Write column select
  switch (g_col) { // clang-format off
  case 0:  PORTD |= 1 << 7; break; // C0
  case 1:  PORTC |= 1 << 2; break; // C1
  case 2:  PORTC |= 1 << 3; break; // C2
  case 3:  PORTC |= 1 << 4; break; // C3
  case 4:  PORTC |= 1 << 5; break; // C4
  case 5:  PORTC |= 1 << 6; break; // C5
  case 6:  PORTC |= 1 << 7; break; // C6
  case 7:  PORTA |= 1 << 6; break; // C7
  case 8:  PORTA |= 1 << 5; break; // C8
  case 9:  PORTA |= 1 << 4; break; // C9
  case 10: PORTA |= 1 << 3; break; // C10
  case 11: PORTA |= 1 << 2; break; // C11
  case 12: PORTA |= 1 << 1; break; // C12
  case 13: PORTA |= 1 << 0; break; // C13
  case 14: PORTB |= 1 << 0; break; // C14
  case 15: PORTB |= 1 << 1; break; // C15
  case 16: PORTB |= 1 << 2; break; // C16
  } // clang-format on

  if (++g_col >= Display::c_width) {
    g_col = 0;
  }
}

ISR(TIMER2_COMP_vect) {
  display_step();
}

ISR(TIMER2_OVF_vect) {
  display_off();
}

Display::Display() {
  display_init();
}

void Display::setDot(uint8_t x, uint8_t y, uint8_t value) {
  g_buf[x][y] = value;
}

uint8_t Display::getDot(uint8_t x, uint8_t y) {
  return g_buf[x][y];
}

void Display::setBrigntess(uint8_t brightness) {
  display_set_brigtness(brightness);
}
