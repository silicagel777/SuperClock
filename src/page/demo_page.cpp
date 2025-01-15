#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "page/demo_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

DemoPage::DemoPage(PageManager &pageManager, PageEnv &env)
    : m_pageManager(pageManager), m_env(env) {}

void DemoPage::show() {
  m_env.button.setCallback(buttonCallback, this);
  m_env.sched.addTask(showDemoCallback, this, 0);
}

void DemoPage::hide() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void DemoPage::buttonCallback(void *self, Button::Type type, Button::State state) {
  ((DemoPage *)self)->handleButton(type, state);
}

void DemoPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageManager::PageType::CLOCK_PAGE);
    }
  }
}

void DemoPage::showDemoCallback(void *self) {
  ((DemoPage *)self)->showDemo();
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
  m_env.sched.addTask(showDemoCallback, this, c_demoRefreshDelay);
}
