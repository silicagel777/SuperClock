#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "driver/temp/itemp.h"
#include "page/clock_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

ClockPage::ClockPage(PageManager &pageManager, PageEnv &env)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<ClockPage, &ClockPage::handleButton>(this);
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, 0);
}

ClockPage::~ClockPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void ClockPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageManager::PageType::DEMO_PAGE);
    }
  } else if (type == Button::Type::PLUS) {
    if (state == Button::State::RELEASE) {
      m_env.sched.removeTasks(this);
      showDate();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.sched.removeTasks(this);
      showYear();
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::RELEASE) {
      m_env.sched.removeTasks(this);
      showWeek();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.sched.removeTasks(this);
      showTemp();
    }
  }
}

void ClockPage::showTime() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(&rtcTime);
  m_env.display.clear();
  char s[] = {
      (char)('0' + rtcTime.hour / 10),
      (char)('0' + rtcTime.hour % 10),
      ':',
      (char)('0' + rtcTime.minute / 10),
      (char)('0' + rtcTime.minute % 10),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.writePixel(rtcTime.second / 4 + 1, 5);
  m_env.display.update();
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, c_timeRefreshDelay);
}

void ClockPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(&rtcTime);
  m_env.display.clear();
  char s[] = {
      (char)('0' + rtcTime.month / 10),
      (char)('0' + rtcTime.month % 10),
      '.',
      (char)('0' + rtcTime.day / 10),
      (char)('0' + rtcTime.day % 10),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, c_returnToTimeDelay);
}

void ClockPage::showTemp() {
  int16_t temp;
  m_env.temp.readTemp(&temp);
  temp = temp < 0 ? -temp : temp;
  int8_t tempInt = temp / 100;
  int8_t tempFrac = temp % 100;
  m_env.display.clear();
  char s[] = {
      (char)('0' + tempInt / 10),
      (char)('0' + tempInt % 10),
      '.',
      (char)('0' + tempFrac / 10),
      (char)('0' + tempFrac % 10),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, c_returnToTimeDelay);
}

void ClockPage::showWeek() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(&rtcTime);
  m_env.display.clear();
  char s[] = {
      '0',
      (char)('0' + rtcTime.week),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, c_returnToTimeDelay);
}

void ClockPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(&rtcTime);
  m_env.display.clear();
  char s[] = {
      (char)('0' + rtcTime.year / 1000 % 10),
      (char)('0' + rtcTime.year / 100 % 10),
      (char)('0' + rtcTime.year / 10 % 10),
      (char)('0' + rtcTime.year % 10),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
  m_env.sched.addTask<ClockPage, &ClockPage::showTime>(this, c_returnToTimeDelay);
}
