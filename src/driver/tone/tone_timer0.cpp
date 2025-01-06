#include <avr/io.h>

#include "driver/tone/tone_timer0.h"
#include "tone_timer0.h"

ToneTimer0::ToneTimer0() {
  // CTC mode, F_CPU/256
  TCCR0 = (1 << WGM01) | (1 << CS02);
}

void ToneTimer0::run(uint16_t freq) {
  OCR0 = (F_CPU / 256 / 2) / freq - 1;
  TCCR0 |= 1 << COM00; // Toggle OC0 on compare match
  DDRB |= 1 << PB3;    // Enable OC0 output driver
}

void ToneTimer0::stop() {
  TCCR0 &= ~(1 << COM00);
  DDRB &= ~(1 << PB3);
}
