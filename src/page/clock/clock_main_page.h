#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class ClockMainPage {
public:
  ClockMainPage(PageManager &pageManager, PageEnv &env, uintptr_t arg);
  ~ClockMainPage();

private:
  ClockMainPage(const ClockMainPage &) = delete;
  void operator=(const ClockMainPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showTime();
  void showDate();
  void showTemp();
  void showWeek();
  void showYear();
  static constexpr uint16_t c_timeRefreshDelay = 500;
  static constexpr uint16_t c_returnToTimeDelay = 3000;
  PageManager &m_pageManager;
  PageEnv &m_env;
};
