#pragma once

#include <driver/button/button.h>

class PageManager;
class PageEnv;

class AlarmViewPage {
public:
  AlarmViewPage(PageManager &PageManager, PageEnv &env);
  ~AlarmViewPage();

private:
  AlarmViewPage(const AlarmViewPage &) = delete;
  void operator=(const AlarmViewPage &) = delete;
  void handleButton(Button::Type type, Button::State state);
  void showAlarm();
  void toggleAlarm();
  PageManager &m_pageManager;
  PageEnv &m_env;
  uint8_t m_currentAlarm = 0;
};
