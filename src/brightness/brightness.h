#pragma once

#include <stdint.h>

class Adc;
class Display;
class IFilter;
class Sched;

class Brightness {
public:
  static constexpr uint16_t c_updateDelay = 50;
  Brightness(Sched &sched, Display &display, IFilter &filter, Adc &adc, uint8_t adcChannel);
  ~Brightness();

private:
  Brightness(const Brightness &) = delete;
  void operator=(const Brightness &) = delete;
  uint16_t getInput();
  void update();
  Sched &m_sched;
  Display &m_display;
  IFilter &m_filter;
  Adc &m_adc;
  uint8_t m_adcChannel = 0;
};
