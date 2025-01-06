#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#include "system.h"

#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

static volatile uint32_t milliseconds;

ISR(TIMER1_COMPA_vect) {
  milliseconds++;
}

uint32_t systemMilliseconds(void) {
  uint32_t result;
  do {
    result = milliseconds;
  } while (result != milliseconds);
  return result;
}

void systemInit(void) {
  // CTC mode, Clock/8
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
