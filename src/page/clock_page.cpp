#include <buzzer/buzzer.h>
#include <driver/button/button.h>
#include <driver/display/display.h>
#include <driver/rtc/irtc.h>
#include <sched/sched.h>

#include "page/clock_page.h"

ClockPage::ClockPage(Sched &sched, Display &display, Buzzer &buzzer, Button &button, IRtc &rtc)
    : m_sched(sched), m_display(display), m_buzzer(buzzer), m_button(button), m_rtc(rtc) {}

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
  if (type == Button::Type::PLUS && state == Button::State::RELEASE) {
    m_sched.removeTasks(showTimeCallback, this);
    showDate();
  } else if (type == Button::Type::MINUS && state == Button::State::RELEASE) {
    m_sched.removeTasks(showTimeCallback, this);
    showYear();
  }
}

void ClockPage::showTimeCallback(void *self) {
  ((ClockPage *)self)->showTime();
}

void ClockPage::showTime() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(rtcTime.hr / 10, 0, 0);
  m_display.writeNumber(rtcTime.hr % 10, 4, 0);
  m_display.writeNumber(rtcTime.min / 10, 10, 0);
  m_display.writeNumber(rtcTime.min % 10, 14, 0);
  m_display.writePixel(8, 1);
  m_display.writePixel(8, 4);
  for (uint8_t i = 0; i < Display::c_width; i++) {
    m_display.writePixel(i, 5, 0);
  }
  m_display.writePixel(rtcTime.sec / 4 + 1, 5);
  m_sched.addTask(showTimeCallback, this, c_timeRefreshDelay);
}

void ClockPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(rtcTime.mon / 10, 0, 0);
  m_display.writeNumber(rtcTime.mon % 10, 4, 0);
  m_display.writeNumber(rtcTime.date / 10, 10, 0);
  m_display.writeNumber(rtcTime.date % 10, 14, 0);
  m_display.writePixel(8, 4);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(rtcTime.yr / 1000, 0, 0);
  m_display.writeNumber(rtcTime.yr % 100 % 10, 4, 0);
  m_display.writeNumber(rtcTime.yr / 10 % 10, 10, 0);
  m_display.writeNumber(rtcTime.yr % 10, 14, 0);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}
