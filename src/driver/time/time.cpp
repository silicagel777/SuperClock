#include <avr/interrupt.h>
#include <avr/io.h>

#include "driver/time/time.h"

#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

static volatile uint32_t g_milliseconds;

ISR(TIMER1_COMPA_vect) {
  g_milliseconds++;
}

Time::Time() {
  // Timer 1, CTC mode, Clock/8
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  // Load the high byte, then the low byte
  // into the output compare
  OCR1AH = (CTC_MATCH_OVERFLOW >> 8) & 0xFF;
  OCR1AL = CTC_MATCH_OVERFLOW & 0xFF;
  // Enable the compare match interrupt
  TIMSK |= (1 << OCIE1A);
  // Now enable global interrupts
  sei();
}

uint32_t Time::milliseconds() {
  uint32_t result;
  do {
    result = g_milliseconds;
  } while (result != g_milliseconds);
  return result;
}