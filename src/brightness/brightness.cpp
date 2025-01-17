#include <driver/adc/adc.h>
#include <driver/display/display.h>
#include <sched/sched.h>

#include "brightness/brightness.h"

Brightness::Brightness(Sched &sched, Display &display, Adc &adc, uint8_t adcChannel)
    : m_sched(sched), m_display(display), m_adc(adc), m_adcChannel(adcChannel) {
  m_sched.addTask<Brightness, &Brightness::update>(this, 0, c_updateDelay);
}

Brightness::~Brightness() {
  m_sched.removeTasks(this);
}

void Brightness::update() {
  constexpr uint8_t scale = (Adc::c_maxValue + 1) / (Display::c_maxGlobalBrightness + 1);
  m_display.setGlobalBrightness(m_adc.read(m_adcChannel) / scale);
}
