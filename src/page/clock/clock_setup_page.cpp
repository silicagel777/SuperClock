#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "page/clock/clock_setup_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

ClockSetupPage::ClockSetupPage(PageManager &pageManager, PageEnv &env, uintptr_t)
    : m_pageManager(pageManager), m_env(env) {
  m_env.rtc.readTime(m_time);
  m_time.second = 0;
  m_env.button.setCallback<ClockSetupPage, &ClockSetupPage::handleButton>(this);
  m_env.sched.addTask<ClockSetupPage, &ClockSetupPage::showSetup>(this, 0, c_setupRefreshDelay);
}

ClockSetupPage::~ClockSetupPage() {
  m_env.rtc.setTime(m_time);
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void ClockSetupPage::handleButton(Button::Type type, Button::State state) {
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_env.buzzer.beep();
      if (m_mode == Mode::YEAR) {
        m_pageManager.changePage(PageType::CLOCK_MAIN_PAGE);
      } else {
        ((uint8_t &)m_mode)++;
        m_blinkFlag = true;
        setupRefresh();
      }
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_pageManager.changePage(PageType::CLOCK_MAIN_PAGE);
    }
  } else if (type == Button::Type::PLUS) {
    if (state == Button::State::PRESS) {
      m_env.buzzer.beep();
      setupIncrease();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_env.sched.addTask<ClockSetupPage, &ClockSetupPage::setupIncrease>(
          this, 0, c_autoChangeDelay);
    } else if (state == Button::State::LONG_RELEASE) {
      m_env.sched.removeTasks<ClockSetupPage, &ClockSetupPage::setupIncrease>(this);
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::PRESS) {
      m_env.buzzer.beep();
      setupDecrease();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_env.sched.addTask<ClockSetupPage, &ClockSetupPage::setupDecrease>(
          this, 0, c_autoChangeDelay);
    } else if (state == Button::State::LONG_RELEASE) {
      m_env.sched.removeTasks<ClockSetupPage, &ClockSetupPage::setupDecrease>(this);
    }
  }
}

void ClockSetupPage::setupIncrease() {
  switch (m_mode) {
  case Mode::HOURS:
    m_time.hour = m_time.hour < 23 ? m_time.hour + 1 : 0;
    break;
  case Mode::MINUTES:
    m_time.minute = m_time.minute < 59 ? m_time.minute + 1 : 0;
    break;
  case Mode::DAY:
    m_time.day = m_time.day < getMonthDays(m_time.month, m_time.year) ? m_time.day + 1 : 1;
    break;
  case Mode::MONTH:
    m_time.month = m_time.month < 12 ? m_time.month + 1 : 1;
    m_time.day = m_time.day > getMonthDays(m_time.month, m_time.year) ? 1 : m_time.day;
    break;
  case Mode::YEAR:
    m_time.year = m_time.year < 2100 ? m_time.year + 1 : 2000;
    m_time.day = m_time.day > getMonthDays(m_time.month, m_time.year) ? 1 : m_time.day;
    break;
  }
  m_time.week = getWeek(m_time.day, m_time.month, m_time.year);
  setupRefresh();
}

void ClockSetupPage::setupDecrease() {
  switch (m_mode) {
  case Mode::HOURS:
    m_time.hour = m_time.hour > 0 ? m_time.hour - 1 : 23;
    break;
  case Mode::MINUTES:
    m_time.minute = m_time.minute > 0 ? m_time.minute - 1 : 59;
    break;
  case Mode::DAY:
    m_time.day = m_time.day > 1 ? m_time.day - 1 : getMonthDays(m_time.month, m_time.year);
    break;
  case Mode::MONTH:
    m_time.month = m_time.month > 1 ? m_time.month - 1 : 12;
    m_time.day = m_time.day > getMonthDays(m_time.month, m_time.year) ? 1 : m_time.day;
    break;
  case Mode::YEAR:
    m_time.year = m_time.year > 2000 ? m_time.year - 1 : 2100;
    m_time.day = m_time.day > getMonthDays(m_time.month, m_time.year) ? 1 : m_time.day;
    break;
  }
  m_time.week = getWeek(m_time.day, m_time.month, m_time.year);
  setupRefresh();
}

void ClockSetupPage::setupRefresh() {
  m_env.display.clear();
  switch (m_mode) {
  case Mode::HOURS:
    m_env.display.writeClockNums(m_time.hour, ':', m_time.minute);
    if (m_blinkFlag) {
      m_env.display.writeBottomLine(0, 6);
    }
    break;
  case Mode::MINUTES:
    m_env.display.writeClockNums(m_time.hour, ':', m_time.minute);
    if (m_blinkFlag) {
      m_env.display.writeBottomLine(10, 16);
    }
    break;
  case Mode::DAY:
    m_env.display.writeClockNums(m_time.month, '.', m_time.day);
    if (m_blinkFlag) {
      m_env.display.writeBottomLine(0, 6);
    }
    break;
  case Mode::MONTH:
    m_env.display.writeClockNums(m_time.month, '.', m_time.day);
    if (m_blinkFlag) {
      m_env.display.writeBottomLine(10, 16);
    }
    break;
  case Mode::YEAR:
    m_env.display.writeYearNum(m_time.year);
    if (m_blinkFlag) {
      m_env.display.writeBottomLine(1, 15);
    }
    break;
  }
  m_env.display.update();
}

void ClockSetupPage::showSetup() {
  setupRefresh();
  m_blinkFlag = !m_blinkFlag;
}

uint8_t ClockSetupPage::getMonthDays(const uint8_t month, const uint16_t year) {
  if (month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  } else if (month == 2) {
    bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    return leapYear ? 29 : 28;
  } else {
    return 31;
  }
}

uint8_t ClockSetupPage::getWeek(const uint8_t day, const uint8_t month, const uint16_t year) {
  // Using Zeller's Rule
  uint16_t adjustment, mm, yy;
  adjustment = (14 - month) / 12;
  mm = month + 12 * adjustment - 2;
  yy = year - adjustment;
  return ((day + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) - 1) % 7 + 1;
}
