#include <stdio.h>

#include "buzzer/buzzer.h"
#include "driver/button/button.h"
#include "driver/display/display.h"
#include "driver/rtc/irtc.h"
#include "driver/temp/itemp.h"
#include "page/alarm/alarm_alert_page.h"
#include "page/page_manager.h"
#include "sched/sched.h"

AlarmAlertPage::AlarmAlertPage(PageManager &pageManager, PageEnv &env, uintptr_t)
    : m_pageManager(pageManager), m_env(env) {
  m_env.button.setCallback<AlarmAlertPage, &AlarmAlertPage::handleButton>(this);
  m_env.sched.addTask<AlarmAlertPage, &AlarmAlertPage::showAlert>(this, 0, c_alertRefreshDelay);
}

AlarmAlertPage::~AlarmAlertPage() {
  m_env.button.resetCallback();
  m_env.sched.removeTasks(this);
}

void AlarmAlertPage::handleButton(Button::Type type, Button::State state) {
  if (state == Button::State::RELEASE) {
    m_env.buzzer.beep();
    m_pageManager.changePage(PageType::CLOCK_MAIN_PAGE);
  }
}

void AlarmAlertPage::showAlert() {
  IRtc::RtcTime rtcTime{};
  m_env.rtc.readTime(rtcTime);
  m_env.display.clear();
  m_env.display.writeClockNums(rtcTime.hour, ':', rtcTime.minute);
  m_env.display.writePixel(m_counter, Display::c_maxY);
  m_counter = m_counter >= Display::c_maxX ? 0 : m_counter + 1;
  m_env.display.update();
}
