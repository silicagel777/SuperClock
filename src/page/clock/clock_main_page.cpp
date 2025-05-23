#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "driver/temp/itemp.h"
#include "page/clock/clock_main_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

ClockMainPage::ClockMainPage(PageManager &pageManager, PageEnv &env, uintptr_t)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<ClockMainPage, &ClockMainPage::handleButton>(this);
  m_env.sched.addTask<ClockMainPage, &ClockMainPage::showTime>(this, 0, c_timeRefreshDelay);
}

ClockMainPage::~ClockMainPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void ClockMainPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageType::ALARM_VIEW_PAGE);
    } else if (state == Button::State::LONG_PRESS) {
      m_pageManager.changePage(PageType::CLOCK_SETUP_PAGE);
    }
  } else if (type == Button::Type::PLUS) {
    if (state == Button::State::RELEASE) {
      m_env.sched.removeTasks<ClockMainPage, &ClockMainPage::showTime>(this);
      m_env.sched.addTask<ClockMainPage, &ClockMainPage::showTime>(
          this, c_returnToTimeDelay, c_timeRefreshDelay);
      showDate();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.sched.removeTasks<ClockMainPage, &ClockMainPage::showTime>(this);
      m_env.sched.addTask<ClockMainPage, &ClockMainPage::showTime>(
          this, c_returnToTimeDelay, c_timeRefreshDelay);
      showYear();
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::RELEASE) {
      m_env.sched.removeTasks<ClockMainPage, &ClockMainPage::showTime>(this);
      m_env.sched.addTask<ClockMainPage, &ClockMainPage::showTime>(
          this, c_returnToTimeDelay, c_timeRefreshDelay);
      showWeek();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.sched.removeTasks<ClockMainPage, &ClockMainPage::showTime>(this);
      m_env.sched.addTask<ClockMainPage, &ClockMainPage::showTime>(
          this, c_returnToTimeDelay, c_timeRefreshDelay);
      showTemp();
    }
  }
}

void ClockMainPage::showTime() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(rtcTime);
  m_env.display.clear();
  m_env.display.writeClockNums(rtcTime.hour, ':', rtcTime.minute);
  m_env.display.writePixel(rtcTime.second / 4 + 1, Display::c_maxY);
  m_env.display.update();
}

void ClockMainPage::showDate() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(rtcTime);
  m_env.display.clear();
  m_env.display.writeClockNums(rtcTime.day, '.', rtcTime.month);
  m_env.display.update();
}

void ClockMainPage::showTemp() {
  ITemp::TempValue tempValue;
  m_env.temp.readTemp(tempValue);
  int8_t sign = tempValue.integer < 0 ? -1 : 1;
  m_env.display.clear();
  char s[] = {
      sign < 0 ? '-' : '+',
      (char)('0' + sign * tempValue.integer / 10),
      (char)('0' + sign * tempValue.integer % 10),
      '\xB0', // degree sign
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
}

void ClockMainPage::showWeek() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(rtcTime);
  m_env.display.clear();
  const char *labels[] = {"MON ", "TUE ", "WED ", "THU ", "FRI ", "SAT ", "SUN "};
  m_env.display.writeString(
      labels[rtcTime.week - 1], Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
}

void ClockMainPage::showYear() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(rtcTime);
  m_env.display.clear();
  m_env.display.writeYearNum(rtcTime.year);
  m_env.display.update();
}
