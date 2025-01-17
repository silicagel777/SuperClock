#include <avr/io.h>

#include "driver/tone/tone.h"

Tone::Tone() {
  // Timer 0, CTC mode, F_CPU/256
  TCCR0 = (1 << WGM01) | (1 << CS02);
}

Tone::~Tone() {
  TCCR0 = 0;
  OCR0 = 0;
  DDRB &= ~(1 << PB3);
}

void Tone::run(uint16_t freq) {
  OCR0 = (F_CPU / 256 / 2) / freq - 1;
  TCCR0 |= 1 << COM00; // Toggle OC0 on compare match
  DDRB |= 1 << PB3;    // Enable OC0 output driver
}

void Tone::stop() {
  TCCR0 &= ~(1 << COM00);
  DDRB &= ~(1 << PB3);
}
