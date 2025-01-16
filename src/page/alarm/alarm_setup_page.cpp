#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "page/alarm/alarm_setup_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

AlarmSetupPage::AlarmSetupPage(PageManager &pageManager, PageEnv &env, uintptr_t arg)
    : m_pageManager(pageManager), m_env(env), m_currentAlarm(arg) {
  m_env.alarm.readAlarm(m_currentAlarm, m_time);
  m_env.button.setCallback<AlarmSetupPage, &AlarmSetupPage::handleButton>(this);
  m_env.sched.addTask<AlarmSetupPage, &AlarmSetupPage::showSetup>(this, 0, c_setupRefreshDelay);
}

AlarmSetupPage::~AlarmSetupPage() {
  m_env.alarm.setAlarm(m_currentAlarm, m_time);
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void AlarmSetupPage::handleButton(Button::Type type, Button::State state) {
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_env.buzzer.beep();
      if (m_mode == Mode::MINUTES) {
        m_pageManager.changePage(PageType::ALARM_VIEW_PAGE, m_currentAlarm);
      } else {
        ((uint8_t &)m_mode)++;
        m_blinkFlag = true;
        setupRefresh();
      }
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_pageManager.changePage(PageType::ALARM_VIEW_PAGE, m_currentAlarm);
    }
  } else if (type == Button::Type::PLUS) {
    if (state == Button::State::PRESS) {
      m_env.buzzer.beep();
      setupIncrease();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_env.sched.addTask<AlarmSetupPage, &AlarmSetupPage::setupIncrease>(
          this, 0, c_autoChangeDelay);
    } else if (state == Button::State::LONG_RELEASE) {
      m_env.sched.removeTasks<AlarmSetupPage, &AlarmSetupPage::setupIncrease>(this);
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::PRESS) {
      m_env.buzzer.beep();
      setupDecrease();
    } else if (state == Button::State::LONG_PRESS) {
      m_env.buzzer.beep();
      m_env.sched.addTask<AlarmSetupPage, &AlarmSetupPage::setupDecrease>(
          this, 0, c_autoChangeDelay);
    } else if (state == Button::State::LONG_RELEASE) {
      m_env.sched.removeTasks<AlarmSetupPage, &AlarmSetupPage::setupDecrease>(this);
    }
  }
}

void AlarmSetupPage::setupIncrease() {
  switch (m_mode) {
  case Mode::HOURS:
    m_time.hour = m_time.hour < 23 ? m_time.hour + 1 : 0;
    break;
  case Mode::MINUTES:
    m_time.minute = m_time.minute < 59 ? m_time.minute + 1 : 0;
    break;
  }
  setupRefresh();
}

void AlarmSetupPage::setupDecrease() {
  switch (m_mode) {
  case Mode::HOURS:
    m_time.hour = m_time.hour > 0 ? m_time.hour - 1 : 23;
    break;
  case Mode::MINUTES:
    m_time.minute = m_time.minute > 0 ? m_time.minute - 1 : 59;
    break;
  }
  setupRefresh();
}

void AlarmSetupPage::setupRefresh() {
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
  }
  m_env.display.update();
}

void AlarmSetupPage::showSetup() {
  setupRefresh();
  m_blinkFlag = !m_blinkFlag;
}
