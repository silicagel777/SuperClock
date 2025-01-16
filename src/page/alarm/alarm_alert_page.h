#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class AlarmAlertPage {
public:
  AlarmAlertPage(PageManager &pageManager, PageEnv &env, uintptr_t arg);
  ~AlarmAlertPage();

private:
  AlarmAlertPage(const AlarmAlertPage &) = delete;
  void operator=(const AlarmAlertPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showAlert();
  static constexpr uint16_t c_alertRefreshDelay = 30;
  PageManager &m_pageManager;
  PageEnv &m_env;
  uint8_t m_counter = 0;
};
