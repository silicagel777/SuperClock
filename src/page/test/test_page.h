#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class TestPage {
public:
  TestPage(PageManager &PageManager, PageEnv &env);
  ~TestPage();

private:
  TestPage(const TestPage &) = delete;
  void operator=(const TestPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showTest();
  static constexpr uint16_t c_testRefreshDelay = 40;
  PageManager &m_pageManager;
  PageEnv &m_env;
  uint8_t m_brightness;
  int8_t m_brightnessInc;
};
