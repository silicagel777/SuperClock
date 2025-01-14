#pragma once

#include <driver/button/button.h>
#include <page/ipage.h>

class Buzzer;
class Display;
class IRtc;
class ITemp;
class PageManager;
class Sched;

class DemoPage : public IPage {
public:
  DemoPage(
      PageManager &PageManager, Sched &sched, Display &display, Buzzer &buzzer, Button &button);
  void show() override;
  void hide() override;

private:
  DemoPage(const DemoPage &) = delete;
  void operator=(const DemoPage &) = delete;
  static void buttonCallback(void *self, Button::Type type, Button::State state);
  void handleButton(Button::Type type, Button::State state);
  static void showDemoCallback(void *self);
  void showDemo();
  static constexpr uint16_t c_demoRefreshDelay = 1000;
  PageManager &m_pageManager;
  Sched &m_sched;
  Display &m_display;
  Buzzer &m_buzzer;
  Button &m_button;
  uint16_t m_counter = 0;
};
