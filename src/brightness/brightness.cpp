#include <driver/adc/adc.h>
#include <driver/display/display.h>
#include <filter/ifilter.h>
#include <sched/sched.h>

#include "brightness/brightness.h"

Brightness::Brightness(
    Sched &sched, Display &display, IFilter &filter, Adc &adc, uint8_t adcChannel)
    : m_sched(sched), m_display(display), m_filter(filter), m_adc(adc), m_adcChannel(adcChannel) {
  m_sched.addTask<Brightness, &Brightness::update>(this, 0, c_updateDelay);
  m_filter.reset(getInput());
}

Brightness::~Brightness() {
  m_sched.removeTasks(this);
}

uint16_t Brightness::getInput() {
  constexpr uint8_t scale = (Adc::c_maxValue + 1) / (Display::c_maxGlobalBrightness + 1);
  return m_adc.read(m_adcChannel) / scale;
}

void Brightness::update() {
  uint8_t brightness = m_filter.filter(getInput());
  m_display.setGlobalBrightness(brightness);
}
