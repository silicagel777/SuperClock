#include <avr/io.h>
#include <util/delay.h>

#include "driver/display/display.h"

// TODO: rewrite properly to timer2 interrupts!

Display::Display() {
  // Init outputs
  DDRA |= 0b01111111;
  DDRB |= 0b00010111;
  DDRC |= 0b11111100;
  DDRD |= 0b11111111;
}

void Display::process() {
  // Reset outputs
  PORTA &= ~0b01111111;
  PORTB &= ~0b00010111;
  PORTC &= ~0b11111100;
  PORTD &= ~0b11111111;

  // Write rows
  PORTB |= (m_buf[m_col][1] ? 1 : 0) << 4;  // DOT0
  PORTD |= (m_buf[m_col][4] ? 1 : 0) << 0 | // DOT1
           (m_buf[m_col][5] ? 1 : 0) << 1 | // A5
           (m_buf[m_col][4] ? 1 : 0) << 2 | // A4
           (m_buf[m_col][3] ? 1 : 0) << 3 | // A3
           (m_buf[m_col][2] ? 1 : 0) << 4 | // A2
           (m_buf[m_col][1] ? 1 : 0) << 5 | // A1
           (m_buf[m_col][0] ? 1 : 0) << 6;  // A0

  _delay_us(400); // reduce global brigtness

  // Write column select
  switch (m_col) { // clang-format off
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

  if (++m_col >= c_width) {
    m_col = 0;
  }

  _delay_us(50);
}
