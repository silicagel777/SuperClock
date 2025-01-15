#pragma once

#include <driver/button/button.h>
#include <page/ipage.h>

class PageManager;
class PageEnv;

class ClockPage : public IPage {
public:
  ClockPage(PageManager &pageManager, PageEnv &env);
  void show() override;
  void hide() override;

private:
  ClockPage(const ClockPage &) = delete;
  void operator=(const ClockPage &) = delete;
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
