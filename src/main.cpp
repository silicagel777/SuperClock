#include "buzzer/buzzer.h"
#include "driver/adc/adc.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/i2c/i2c.h"
#include "driver/rtc/ds3231.h"
#include "driver/rtc/irtc.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Time time{};
Sched sched{time};
Display display{};
Tone tone{};
Buzzer buzzer{sched, tone};
I2C i2c{I2C::FreqMode::FREQ_400K};
Ds3231 rtc{i2c};
Adc adc{Adc::ReferenceMode::AVCC, Adc::PrescalerMode::DIV128};
Button button{sched};

void animate(void *p) {
  static uint8_t animateVal = Display::c_maxPixelBrigtness - 1;
  static int8_t animateInc = 1;
  animateVal += animateInc;
  if (animateVal == 0 || animateVal == Display::c_maxPixelBrigtness) {
    animateInc = -animateInc;
  }
  display.writePixel(8, 1, animateVal);
  display.writePixel(8, 4, animateVal);
  sched.addTask(animate, nullptr, 30);
}

void updateTime(void *p) {
  IRtc::RtcTime rtcTime;
  rtc.readTime(&rtcTime);

  display.writeNumber(rtcTime.hr / 10, 0, 0);
  display.writeNumber(rtcTime.hr % 10, 4, 0);
  display.writeNumber(rtcTime.min / 10, 10, 0);
  display.writeNumber(rtcTime.min % 10, 14, 0);
  for (uint8_t i = 0; i < Display::c_width; i++) {
    display.writePixel(i, 5, 0);
  }
  display.writePixel(rtcTime.sec / 4 + 1, 5);

  sched.addTask(updateTime, nullptr, 500);
}

void updateBrigtness(void *p) {
  constexpr uint8_t scale = (Adc::c_maxValue + 1) / (Display::c_maxGlobalBrightness + 1);
  display.setGlobalBrigntess(adc.read(7) / scale);
  sched.addTask(updateBrigtness, nullptr, 50);
}

void reactToButtons(void *p, Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    buzzer.beep();
  }
}

int main(void) {
  button.setCallback(reactToButtons, nullptr);
  sched.addTask(animate, nullptr, 0);
  sched.addTask(updateTime, nullptr, 0);
  sched.addTask(updateBrigtness, nullptr, 0);
  for (;;) {
    sched.process();
  }
}
