#pragma once

#include <stdint.h>

class Sched;

class Button {
public:
  static constexpr uint8_t typeCount = 3;
  enum class Type : uint8_t { MODE, PLUS, MINUS, _TYPE_MAX };
  enum class State { PRESS, RELEASE, LONG_PRESS, LONG_RELEASE };
  typedef void (*button_cb_t)(void *ctx, Type type, State state);

  Button(Sched &sched);
  ~Button();
  bool read(Type type);
  void setCallback(button_cb_t cb, void *ctx);
  void resetCallback();

  template <class C, void (C::*M)(Type type, State state)>
  void setCallback(C *ctx) {
    auto cb = [](void *ctx, Type type, State state) { (static_cast<C *>(ctx)->*M)(type, state); };
    return setCallback(cb, ctx);
  }

private:
  Button(const Button &) = delete;
  void operator=(const Button &) = delete;
  void runCallback(Type type, State state);
  void poll();
  static constexpr uint16_t c_delayCycle = 50;
  static constexpr uint16_t c_longPressCycles = 14;
  static constexpr uint16_t c_maxCycles = 255;
  Sched &m_sched;
  uint8_t m_cycles[typeCount]{};
  button_cb_t m_cb = nullptr;
  void *m_cbCtx = nullptr;
};
