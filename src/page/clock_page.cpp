#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "driver/temp/itemp.h"
#include "page/clock_page.h"
#include "sched/sched.h"

ClockPage::ClockPage(
    Sched &sched, Display &display, Buzzer &buzzer, Button &button, IRtc &rtc, ITemp &temp)
    : m_sched(sched), m_display(display), m_buzzer(buzzer), m_button(button), m_rtc(rtc),
      m_temp(temp) {}

void ClockPage::show() {
  m_button.setCallback(buttonCallback, this);
  m_sched.addTask(showTimeCallback, this, 0);
}

void ClockPage::hide() {
  m_button.resetCallback();
  m_sched.removeTasks(showTimeCallback, this);
}

void ClockPage::buttonCallback(void *self, Button::Type type, Button::State state) {
  ((ClockPage *)self)->handleButton(type, state);
}

void ClockPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_buzzer.beep();
  }
  if (type == Button::Type::PLUS) {
    if (state == Button::State::RELEASE) {
      m_sched.removeTasks(showTimeCallback, this);
      showDate();
    } else if (state == Button::State::LONG_PRESS) {
      m_sched.removeTasks(showTimeCallback, this);
      showYear();
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::RELEASE) {
      m_sched.removeTasks(showTimeCallback, this);
      showWeek();
    } else if (state == Button::State::LONG_PRESS) {
      m_sched.removeTasks(showTimeCallback, this);
      showTemp();
    }
  }
}

void ClockPage::showTimeCallback(void *self) {
  ((ClockPage *)self)->showTime();
}

void ClockPage::showTime() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[6];
  snprintf(s, sizeof(s), "%02d:%02d", rtcTime.hour, rtcTime.minute);
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.writePixel(rtcTime.second / 4 + 1, 5);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_timeRefreshDelay);
}

void ClockPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[6];
  snprintf(s, sizeof(s), "%02d.%02d", rtcTime.month, rtcTime.day);
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showTemp() {
  int16_t temp;
  m_temp.readTemp(&temp);
  temp = temp < 0 ? -temp : temp;
  int8_t tempInt = temp / 100;
  int8_t tempFrac = temp % 100;
  m_display.clear();
  char s[6];
  snprintf(s, sizeof(s), "%02d.%02d", tempInt, tempFrac);
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showWeek() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[3];
  snprintf(s, sizeof(s), "%02d", rtcTime.week);
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[5];
  snprintf(s, sizeof(s), "%04d", rtcTime.year);
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}
