#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "page/demo_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

DemoPage::DemoPage(
    PageManager &pageManager, Sched &sched, Display &display, Buzzer &buzzer, Button &button)
    : m_pageManager(pageManager), m_sched(sched), m_display(display), m_buzzer(buzzer),
      m_button(button) {}

void DemoPage::show() {
  m_button.setCallback(buttonCallback, this);
  m_sched.addTask(showDemoCallback, this, 0);
}

void DemoPage::hide() {
  m_button.resetCallback();
  m_sched.removeTasks(showDemoCallback, this);
}

void DemoPage::buttonCallback(void *self, Button::Type type, Button::State state) {
  ((DemoPage *)self)->handleButton(type, state);
}

void DemoPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_buzzer.beep();
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
  m_display.clear();
  char s[] = {
      (char)('0' + m_counter / 1000 % 10),
      (char)('0' + m_counter / 100 % 10),
      (char)('0' + m_counter / 10 % 10),
      (char)('0' + m_counter % 10),
      '\0',
  };
  m_display.writeString(s, Display::c_centerX, 0, Display::Align::MIDDLE);
  m_display.update();
  if (++m_counter > 9999) {
    m_counter = 0;
  }
  m_sched.addTask(showDemoCallback, this, c_demoRefreshDelay);
}
