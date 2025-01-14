#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "driver/temp/itemp.h"
#include "page/clock_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

ClockPage::ClockPage(PageManager &pageManager, Sched &sched, Display &display, Buzzer &buzzer,
    Button &button, IRtc &rtc, ITemp &temp)
    : m_pageManager(pageManager), m_sched(sched), m_display(display), m_buzzer(buzzer),
      m_button(button), m_rtc(rtc), m_temp(temp) {}

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
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageManager::PageType::DEMO_PAGE);
    }
  } else if (type == Button::Type::PLUS) {
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
  char s[] = {
      (char)('0' + rtcTime.hour / 10),
      (char)('0' + rtcTime.hour % 10),
      ':',
      (char)('0' + rtcTime.minute / 10),
      (char)('0' + rtcTime.minute % 10),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.writePixel(rtcTime.second / 4 + 1, 5);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_timeRefreshDelay);
}

void ClockPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[] = {
      (char)('0' + rtcTime.month / 10),
      (char)('0' + rtcTime.month % 10),
      '.',
      (char)('0' + rtcTime.day / 10),
      (char)('0' + rtcTime.day % 10),
      '\0',
  };
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
  char s[] = {
      (char)('0' + tempInt / 10),
      (char)('0' + tempInt % 10),
      '.',
      (char)('0' + tempFrac / 10),
      (char)('0' + tempFrac % 10),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showWeek() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[] = {
      '0',
      (char)('0' + rtcTime.week),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}

void ClockPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(&rtcTime);
  m_display.clear();
  char s[] = {
      (char)('0' + rtcTime.year / 1000 % 10),
      (char)('0' + rtcTime.year / 100 % 10),
      (char)('0' + rtcTime.year / 10 % 10),
      (char)('0' + rtcTime.year % 10),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  m_sched.addTask(showTimeCallback, this, c_returnToTimeDelay);
}
