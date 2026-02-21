#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#include "driver/display/display.h"
#include "driver/panic/panic.h"

Panic::Panic(Display &display) : m_display(display) {}

Panic::~Panic() {}

void Panic::panic(Error error) {
  m_display.clear();
  char s[] = {
      'E',
      (char)('0' + (uint8_t)error / 100),
      (char)('0' + (uint8_t)error / 10 % 10),
      (char)('0' + (uint8_t)error % 10),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  for (;;) {
    _NOP();
  }
}
