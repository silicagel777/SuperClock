#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class DemoPage {
public:
  DemoPage(PageManager &PageManager, PageEnv &env);
  ~DemoPage();

private:
  DemoPage(const DemoPage &) = delete;
  void operator=(const DemoPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showDemo();
  static constexpr uint16_t c_demoRefreshDelay = 1000;
  PageManager &m_pageManager;
  PageEnv &m_env;
  uint16_t m_counter = 0;
};
