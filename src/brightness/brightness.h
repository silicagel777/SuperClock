#pragma once

#include <stdint.h>

class Adc;
class Display;
class Sched;

class Brightness {
public:
  Brightness(Sched &sched, Display &display, Adc &adc, uint8_t adcChannel);

private:
  Brightness(const Brightness &) = delete;
  void operator=(const Brightness &) = delete;
  void update();
  static constexpr uint16_t c_updateDelay = 50;
  Sched &m_sched;
  Display &m_display;
  Adc &m_adc;
  uint8_t m_adcChannel = 0;
};
