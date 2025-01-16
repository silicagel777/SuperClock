#pragma once

#include <driver/button/button.h>
#include <driver/rtc/irtc.h>

class PageManager;
class PageEnv;

class ClockSetupPage {
public:
  ClockSetupPage(PageManager &pageManager, PageEnv &env);
  ~ClockSetupPage();

private:
  ClockSetupPage(const ClockSetupPage &) = delete;
  void operator=(const ClockSetupPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void setupIncrease();
  void setupAutoIncrease();
  void setupDecrease();
  void setupAutoDecrease();
  void setupRefresh();
  void showSetup();
  static uint8_t getMonthDays(const uint8_t month, const uint16_t year);
  static uint8_t getWeek(const uint8_t day, const uint8_t month, const uint16_t year);
  static constexpr uint16_t c_setupRefreshDelay = 500;
  static constexpr uint16_t c_autoChangeDelay = 150;
  enum class Mode : uint8_t { HOURS, MINUTES, DAY, MONTH, YEAR };
  PageManager &m_pageManager;
  PageEnv &m_env;
  IRtc::RtcTime m_time{};
  Mode m_mode = Mode::HOURS;
  bool m_blinkFlag = true;
};
