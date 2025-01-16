#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "page/page_manager.h"
#include "page/test/test_page.h"
#include "sched/sched.h"

TestPage::TestPage(PageManager &pageManager, PageEnv &env, uintptr_t)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<TestPage, &TestPage::handleButton>(this);
  m_env.sched.addTask<TestPage, &TestPage::showTest>(this, 0, c_testRefreshDelay);
  m_brightness = Display::c_maxPixelBrightness;
  m_brightnessInc = -1;
}

TestPage::~TestPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void TestPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE || state == Button::State::LONG_PRESS) {
    m_env.buzzer.beep();
  }
  if (type == Button::Type::MODE) {
    if (state == Button::State::RELEASE) {
      m_pageManager.changePage(PageType::CLOCK_MAIN_PAGE);
    }
  }
}

void TestPage::showTest() {
  m_env.display.clear();
  m_env.display.writeString("TEST", Display::c_centerX, 0, Display::Align::MIDDLE, m_brightness);
  const uint8_t values[] = {0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0};
  for (uint8_t x = 0; x < sizeof(values); x++) {
    m_env.display.writePixel(x, Display::c_maxY, values[x]);
  }
  m_env.display.update();

  if ((m_brightness == 0 && m_brightnessInc < 0) ||
      (m_brightness == Display::c_maxPixelBrightness && m_brightnessInc > 0)) {
    m_brightnessInc = -m_brightnessInc;
  }
  m_brightness += m_brightnessInc;
}
