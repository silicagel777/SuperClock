#include <avr/io.h>

#include "button.h"
#include "sched/sched.h"

#define UNSIGNED_MAX(var) ((1 << (sizeof(var) * 8)) - 1)

Button::Button(Sched &sched) : m_sched(sched) {
  // Enable pull-ups
  PORTB |= (1 << 5) | (1 << 6) | (1 << 7);

  m_sched.addTask<Button, &Button::poll>(this, 0);
}

void Button::setCallback(button_cb_t cb, void *ctx) {
  m_cb = cb;
  m_cbCtx = ctx;
}

void Button::resetCallback() {
  m_cb = nullptr;
  m_cbCtx = nullptr;
}

inline void Button::runCallback(Type type, State state) {
  if (m_cb) {
    m_cb(m_cbCtx, type, state);
  }
}

bool Button::read(Type type) {
  switch (type) {
  case Type::MODE:
    return !(PINB & (1 << 5));
  case Type::PLUS:
    return !(PINB & (1 << 7));
  case Type::MINUS:
    return !(PINB & (1 << 6));
  default:
    return false;
  }
}

void Button::poll() {
  for (uint8_t i = 0; i < (uint8_t)Type::_TYPE_MAX; i++) {
    bool isPressed = read((Type)i);
    if (isPressed) {
      if (m_cycles[i] == 0) {
        runCallback((Type)i, State::PRESS);
      } else if (m_cycles[i] == c_longPressCycles) {
        runCallback((Type)i, State::LONG_PRESS);
      }
      if (m_cycles[i] < c_maxCycles) {
        m_cycles[i]++;
      }
    } else {
      if (m_cycles[i] >= c_longPressCycles) {
        runCallback((Type)i, State::LONG_RELEASE);
      } else if (m_cycles[i] > 0) {
        runCallback((Type)i, State::RELEASE);
      }
      m_cycles[i] = 0;
    }
  }

  m_sched.addTask<Button, &Button::poll>(this, c_delayCycle);
}
