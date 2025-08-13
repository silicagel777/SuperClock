#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/time/time.h"
#include "page/page_manager.h"
#include "page/stopwatch/stopwatch_page.h"
#include "sched/sched.h"
#include "stopwatch_page.h"

StopWatchPage::StopWatchPage(PageManager &pageManager, PageEnv &env, uintptr_t)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<StopWatchPage, &StopWatchPage::handleButton>(this);
  m_env.sched.addTask<StopWatchPage, &StopWatchPage::showStopwatch>(
      this, 0, c_stopwatchRefreshDelay);
}

StopWatchPage::~StopWatchPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void StopWatchPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageType::TEST_PAGE);
    } else if (state == Button::State::LONG_PRESS) {
      m_mode = Mode::STOP;
      showStopwatch();
    }
  } else if (type == Button::Type::PLUS || type == Button::Type::MINUS) {
    if (state == Button::State::PRESS) {
      switch (m_mode) {
      case Mode::STOP:
        m_startMs = m_env.time.milliseconds();
        m_mode = Mode::RUN;
        break;
      case Mode::RUN:
        m_pauseIntervalMs = m_env.time.milliseconds() - m_startMs;
        m_mode = Mode::PAUSE;
        break;
      case Mode::PAUSE:
        m_mode = Mode::RUN;
        m_startMs = m_env.time.milliseconds() - m_pauseIntervalMs;
        break;
      };
      showStopwatch();
    }
  }
}

void StopWatchPage::showStopwatch() {
  uint32_t countMs;
  switch (m_mode) {
  case Mode::STOP:
    countMs = 0;
    break;
  case Mode::RUN:
    countMs = m_env.time.milliseconds() - m_startMs;
    if (countMs >= c_stopwatchOverflowMs) {
      countMs -= c_stopwatchOverflowMs;
      m_startMs += c_stopwatchOverflowMs;
    }
    break;
  case Mode::PAUSE:
    countMs = m_pauseIntervalMs;
    break;
  };

  uint16_t seconds = countMs / 1000;
  uint16_t milliseconds = countMs % 1000;
  m_env.display.clear();
  m_env.display.writeClockNums(seconds / 60, ':', seconds % 60);
  m_env.display.writePixel(Display::c_width * milliseconds / 1000, Display::c_maxY);
  m_env.display.update();
}
