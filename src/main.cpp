#include "brightness/brightness.h"
#include "buzzer/buzzer.h"
#include "driver/adc/adc.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/i2c/i2c.h"
#include "driver/rtc/ds3231.h"
#include "driver/rtc/irtc.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "page/clock_page.h"
#include "sched/sched.h"

int main(void) {
  Time time{};
  Sched sched{time};
  Display display{};
  Tone tone{};
  Buzzer buzzer{sched, tone};
  I2C i2c{I2C::FreqMode::FREQ_400K};
  Ds3231 rtcTemp{i2c};
  Adc adc{Adc::ReferenceMode::AVCC, Adc::PrescalerMode::DIV128};
  constexpr uint8_t brigthnessAdcChannel = 7;
  Brightness brightness{sched, display, adc, brigthnessAdcChannel};
  Button button{sched};
  ClockPage clockPage{sched, display, buzzer, button, rtcTemp, rtcTemp};

  clockPage.show();
  for (;;) {
    sched.run();
  }
}
