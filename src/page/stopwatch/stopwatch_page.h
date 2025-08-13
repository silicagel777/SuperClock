#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class StopWatchPage {
public:
  StopWatchPage(PageManager &PageManager, PageEnv &env, uintptr_t arg);
  ~StopWatchPage();

private:
  StopWatchPage(const StopWatchPage &) = delete;
  void operator=(const StopWatchPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showStopwatch();
  static constexpr uint16_t c_stopwatchRefreshDelay = 40;
  static constexpr uint32_t c_stopwatchOverflowMs = 100 * 60 * 1000ul;
  enum class Mode : uint8_t { STOP, RUN, PAUSE };
  PageManager &m_pageManager;
  PageEnv &m_env;
  Mode m_mode = Mode::STOP;
  uint32_t m_startMs = 0;
  uint32_t m_pauseIntervalMs = 0;
};
