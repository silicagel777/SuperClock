#include <buzzer/buzzer.h>
#include <driver/button/button.h>
#include <driver/display/display.h>
#include <driver/rtc/irtc.h>
#include <driver/temp/itemp.h>
#include <sched/sched.h>

#include "clock_page.h"
#include "page/clock_page.h"

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
  m_display.writeNumber(rtcTime.hour / 10, 0, 0);
  m_display.writeNumber(rtcTime.hour % 10, 4, 0);
  m_display.writeNumber(rtcTime.minute / 10, 10, 0);
  m_display.writeNumber(rtcTime.minute % 10, 14, 0);
  m_display.writePixel(8, 1);
  m_display.writePixel(8, 4);
  for (uint8_t i = 0; i < Display::c_width; i++) {
    m_display.writePixel(i, 5, 0);
  }
  m_display.writePixel(rtcTime.second / 4 + 1, 5);
  m_sched.addTask(showTimeCallback, this, c_timeRefreshDelay);
}

void ClockPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(rtcTime.month / 10, 0, 0);
  m_display.writeNumber(rtcTime.month % 10, 4, 0);
  m_display.writeNumber(rtcTime.day / 10, 10, 0);
  m_display.writeNumber(rtcTime.day % 10, 14, 0);
  m_display.writePixel(8, 4);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showTemp() {
  int16_t temp;
  m_temp.readTemp(&temp);
  temp = temp < 0 ? -temp : temp;
  int8_t tempInt = temp / 100;
  int8_t tempFrac = temp % 100;
  m_display.clear();
  m_display.writeNumber(tempInt / 10, 0, 0);
  m_display.writeNumber(tempInt % 10, 4, 0);
  m_display.writeNumber(tempFrac / 10, 10, 0);
  m_display.writeNumber(tempFrac % 10, 14, 0);
  m_display.writePixel(8, 4);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showWeek() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(0, 5, 0);
  m_display.writeNumber(rtcTime.week, 9, 0);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  m_display.writeNumber(rtcTime.year / 1000, 1, 0);
  m_display.writeNumber(rtcTime.year % 100 % 10, 5, 0);
  m_display.writeNumber(rtcTime.year / 10 % 10, 9, 0);
  m_display.writeNumber(rtcTime.year % 10, 13, 0);
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}
