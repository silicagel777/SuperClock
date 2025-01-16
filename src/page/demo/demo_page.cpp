#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "page/demo/demo_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

DemoPage::DemoPage(PageManager &pageManager, PageEnv &env)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<DemoPage, &DemoPage::handleButton>(this);
  m_env.sched.addTask<DemoPage, &DemoPage::showDemo>(this, 0);
}

DemoPage::~DemoPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void DemoPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageType::CLOCK_MAIN_PAGE);
    }
  }
}

void DemoPage::showDemo() {
  m_env.display.clear();
  char s[] = {
      (char)('0' + m_counter / 1000 % 10),
      (char)('0' + m_counter / 100 % 10),
      (char)('0' + m_counter / 10 % 10),
      (char)('0' + m_counter % 10),
      '\0',
  };
  m_env.display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_env.display.update();
  if (++m_counter > 9999) {
    m_counter = 0;
  }
  m_env.sched.addTask<DemoPage, &DemoPage::showDemo>(this, c_demoRefreshDelay);
}
