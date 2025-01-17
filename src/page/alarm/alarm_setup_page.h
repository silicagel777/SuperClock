#pragma once

#include <alarm/alarm.h>
#include <driver/button/button.h>

class PageManager;
class PageEnv;

class AlarmSetupPage {
public:
  AlarmSetupPage(PageManager &pageManager, PageEnv &env, uintptr_t arg);
  ~AlarmSetupPage();

private:
  AlarmSetupPage(const AlarmSetupPage &) = delete;
  void operator=(const AlarmSetupPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void setupIncrease();
  void setupDecrease();
  void setupRefresh();
  void showSetup();
  static constexpr uint16_t c_setupRefreshDelay = 500;
  static constexpr uint16_t c_autoChangeDelay = 150;
  enum class Mode : uint8_t { HOURS, MINUTES };
  PageManager &m_pageManager;
  PageEnv &m_env;
  Alarm::AlarmTime m_time{};
  uint8_t m_currentAlarm = 0;
  Mode m_mode = Mode::HOURS;
  bool m_blinkFlag = true;
};
