#include <stdio.h>

#include "alarm/alarm.h"
#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "page/alarm/alarm_view_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

AlarmViewPage::AlarmViewPage(PageManager &pageManager, PageEnv &env, uintptr_t arg)
    : m_pageManager(pageManager), m_env(env), m_currentAlarm(arg) {
  m_env.button.setCallback<AlarmViewPage, &AlarmViewPage::handleButton>(this);
  m_env.sched.addTask<AlarmViewPage, &AlarmViewPage::showAlarm>(this, 0);
}

AlarmViewPage::~AlarmViewPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void AlarmViewPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageType::TEST_PAGE);
    } else if (state == Button::State::LONG_PRESS) {
      m_pageManager.changePage(PageType::ALARM_SETUP_PAGE, m_currentAlarm);
    }
  } else if (type == Button::Type::PLUS) {
    if (state == Button::State::RELEASE) {
      m_currentAlarm = m_currentAlarm < Alarm::c_alarmMax ? m_currentAlarm + 1 : 0;
    } else if (state == Button::State::LONG_PRESS) {
      toggleAlarm();
    }
  } else if (type == Button::Type::MINUS) {
    if (state == Button::State::RELEASE) {
      m_currentAlarm = m_currentAlarm > 0 ? m_currentAlarm - 1 : Alarm::c_alarmMax;
    } else if (state == Button::State::LONG_PRESS) {
      toggleAlarm();
    }
  }
  showAlarm();
}

void AlarmViewPage::showAlarm() {
  Alarm::AlarmTime alarmTime{};
  m_env.alarm.readAlarm(m_currentAlarm, alarmTime);
  m_env.display.clear();
  m_env.display.writeClockNums(alarmTime.hour, ':', alarmTime.minute);
  constexpr uint8_t lineLength = Display::c_width / Alarm::c_alarmCount;
  const uint8_t lineStart = 1 + m_currentAlarm * lineLength;
  m_env.display.writeBottomLine(lineStart, lineStart + lineLength - 1);
  if (alarmTime.enabled) {
    m_env.display.writePixel(Display::c_centerX, 2);
    m_env.display.writePixel(Display::c_centerX, 3);
  }
  m_env.display.update();
}

void AlarmViewPage::toggleAlarm() {
  Alarm::AlarmTime alarmTime{};
  m_env.alarm.readAlarm(m_currentAlarm, alarmTime);
  alarmTime.enabled = !alarmTime.enabled;
  m_env.alarm.setAlarm(m_currentAlarm, alarmTime);
}
