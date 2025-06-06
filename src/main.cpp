#include "alarm/alarm.h"
#include "brightness/brightness.h"
#include "buzzer/buzzer.h"
#include "driver/adc/adc.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/i2c/i2c.h"
#include "driver/init/init.h"
#include "driver/rtc/buffered_rtc.h"
#include "driver/rtc/ds3231.h"
#include "driver/rtc/irtc.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "filter/exponential_filter.h"
#include "page/page_manager.h"
#include "sched/sched.h"

int main(void) {
  Init init{};
  Time time{};
  Sched sched{time};
  Display display{};
  Tone tone{};
  Buzzer buzzer{sched, tone};
  I2C i2c{I2C::FreqMode::FREQ_400K};
  Ds3231 Ds3231{i2c};
  BufferedRtc rtc{sched, Ds3231};
  Adc adc{Adc::ReferenceMode::AVCC, Adc::PrescalerMode::DIV128};
  constexpr uint8_t brightnessAdcChannel = 7;
  ExponentialFilter filter{5.0f, Brightness::c_updateDelay / 1000.0f};
  Brightness brightness{sched, display, filter, adc, brightnessAdcChannel};
  Button button{sched};
  Alarm alarm{sched, rtc};
  PageEnv pageEnv{sched, display, buzzer, button, rtc, Ds3231, alarm};
  constexpr PageType startPageType = PageType::CLOCK_MAIN_PAGE;
  PageManager pageManager{pageEnv, startPageType};

  const auto alarmCb = [](void *ctx) {
    static_cast<PageManager *>(ctx)->changePage(PageType::ALARM_ALERT_PAGE);
  };
  alarm.setCallback(alarmCb, &pageManager);

  for (;;) {
    sched.run();
  }
}
