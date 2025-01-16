#include <avr/interrupt.h>
#include <avr/io.h>

#include "driver/time/time.h"

static constexpr uint16_t c_ctcMatchOverflow = (F_CPU / 1000) / 8;
static volatile uint16_t g_milliseconds = 0;

ISR(TIMER1_COMPA_vect) {
  g_milliseconds++;
}

Time::Time() {
  // Timer 1, CTC mode, F_CPU/8
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  // Load the high byte, then the low byte
  // into the output compare
  OCR1AH = (c_ctcMatchOverflow >> 8) & 0xFF;
  OCR1AL = c_ctcMatchOverflow & 0xFF;
  // Enable the compare match interrupt
  TIMSK |= (1 << OCIE1A);
  // Now enable global interrupts
  sei();
}

uint16_t Time::milliseconds() {
  uint16_t result;
  do {
    result = g_milliseconds;
  } while (result != g_milliseconds);
  return result;
}
