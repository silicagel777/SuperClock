#include <avr/io.h>
#include <util/delay.h>

#include "display.h"

// TODO: rewrite properly to timer2 interrupts!

#define set_bit(byte, bit) ((byte) |= 1 << (bit))
#define unset_bit(byte, bit) ((byte) &= ~(1 << (bit)))
#define toggle_bit(byte, bit) ((byte) ^= 1 << (bit))
#define write_bit(byte, bit, value) ((value) ? set_bit((byte), (bit)) : unset_bit((byte), (bit)))
#define get_bit(byte, bit) (((byte) >> (bit)) & 1)

static const uint8_t buf[17][6] = {
    {0, 0, 0, 0, 0, 0},
    {0, 255, 0, 0, 0, 0},
    {255, 255, 255, 255, 255, 0},
    {0, 0, 0, 0, 0, 0},
    {255, 0, 255, 0, 255, 0},
    {255, 0, 255, 0, 255, 0},
    {255, 255, 255, 255, 255, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 255, 0, 0, 255, 0},
    {0, 0, 0, 0, 0, 0},
    {255, 0, 255, 255, 255, 0},
    {255, 0, 255, 0, 255, 0},
    {255, 255, 255, 0, 255, 0},
    {0, 0, 0, 0, 0, 0},
    {255, 0, 255, 0, 255, 0},
    {255, 0, 255, 0, 255, 255},
    {255, 255, 255, 255, 255, 0},
};

Display::Display() {
  set_bit(DDRD, 6); // A0
  set_bit(DDRD, 5); // A1
  set_bit(DDRD, 4); // A2
  set_bit(DDRD, 3); // A3
  set_bit(DDRD, 2); // A4
  set_bit(DDRD, 1); // A5
  set_bit(DDRB, 4); // DOT0
  set_bit(DDRD, 0); // DOT1

  set_bit(DDRD, 7); // C0
  set_bit(DDRC, 2); // C1
  set_bit(DDRC, 3); // C2
  set_bit(DDRC, 4); // C3
  set_bit(DDRC, 5); // C4
  set_bit(DDRC, 6); // C5
  set_bit(DDRC, 7); // C6
  set_bit(DDRA, 6); // C7
  set_bit(DDRA, 5); // C8
  set_bit(DDRA, 4); // C9
  set_bit(DDRA, 3); // C10
  set_bit(DDRA, 2); // C11
  set_bit(DDRA, 1); // C12
  set_bit(DDRA, 0); // C13
  set_bit(DDRB, 0); // C14
  set_bit(DDRB, 1); // C15
  set_bit(DDRB, 2); // C16
}

void Display::process() {
  unset_bit(PORTD, 7); // C0
  unset_bit(PORTC, 2); // C1
  unset_bit(PORTC, 3); // C2
  unset_bit(PORTC, 4); // C3
  unset_bit(PORTC, 5); // C4
  unset_bit(PORTC, 6); // C5
  unset_bit(PORTC, 7); // C6
  unset_bit(PORTA, 6); // C7
  unset_bit(PORTA, 5); // C8
  unset_bit(PORTA, 4); // C9
  unset_bit(PORTA, 3); // C10
  unset_bit(PORTA, 2); // C11
  unset_bit(PORTA, 1); // C12
  unset_bit(PORTA, 0); // C13
  unset_bit(PORTB, 0); // C14
  unset_bit(PORTB, 1); // C15
  unset_bit(PORTB, 2); // C16

  write_bit(PORTD, 6, buf[m_col][0]); // A0
  write_bit(PORTD, 5, buf[m_col][1]); // A1
  write_bit(PORTD, 4, buf[m_col][2]); // A2
  write_bit(PORTD, 3, buf[m_col][3]); // A3
  write_bit(PORTD, 2, buf[m_col][4]); // A4
  write_bit(PORTD, 1, buf[m_col][5]); // A5

  _delay_us(400); // reduce global brigtness

  switch (m_col) {
  case 0:
    set_bit(PORTD, 7);
    break;
  case 1:
    set_bit(PORTC, 2);
    break;
  case 2:
    set_bit(PORTC, 3);
    break;
  case 3:
    set_bit(PORTC, 4);
    break;
  case 4:
    set_bit(PORTC, 5);
    break;
  case 5:
    set_bit(PORTC, 6);
    break;
  case 6:
    set_bit(PORTC, 7);
    break;
  case 7:
    set_bit(PORTA, 6);
    break;
  case 8:
    set_bit(PORTA, 5);
    break;
  case 9:
    set_bit(PORTA, 4);
    break;
  case 10:
    set_bit(PORTA, 3);
    break;
  case 11:
    set_bit(PORTA, 2);
    break;
  case 12:
    set_bit(PORTA, 1);
    break;
  case 13:
    set_bit(PORTA, 0);
    break;
  case 14:
    set_bit(PORTB, 0);
    break;
  case 15:
    set_bit(PORTB, 1);
    break;
  case 16:
    set_bit(PORTB, 2);
    break;
  }

  if (++m_col > 16) {
    m_col = 0;
  }

  _delay_us(50);
}
