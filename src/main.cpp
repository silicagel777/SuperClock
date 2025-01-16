#include "brightness/brightness.h"
#include "buzzer/buzzer.h"
#include "driver/adc/adc.h"
#include "driver/alarm/alarm.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/i2c/i2c.h"
#include "driver/rtc/buffered_rtc.h"
#include "driver/rtc/ds3231.h"
#include "driver/rtc/irtc.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "page/page_manager.h"
#include "sched/sched.h"

int main(void) {
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
  Brightness brightness{sched, display, adc, brightnessAdcChannel};
  Button button{sched};
  Alarm alarm{sched, rtc};
  PageEnv pageEnv{sched, display, buzzer, button, rtc, Ds3231};
  constexpr PageType startPageType = PageType::CLOCK_MAIN_PAGE;
  PageManager pageManager{pageEnv, startPageType};

  auto alarmCb = [](void *p) { static_cast<PageManager *>(p)->changePage(PageType::DEMO_PAGE); };
  alarm.setCallback(alarmCb, &pageManager);
  for (;;) {
    sched.run();
  }
}
