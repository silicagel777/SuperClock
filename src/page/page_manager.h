#pragma once

#include "page/alarm/alarm_alert_page.h"
#include "page/alarm/alarm_view_page.h"
#include "page/alarm/alarm_setup_page.h"
#include "page/clock/clock_main_page.h"
#include "page/clock/clock_setup_page.h"
#include "page/test/test_page.h"

class Sched;
class Display;
class Buzzer;
class Button;
class IRtc;
class ITemp;
class Alarm;

struct PageEnv {
  Sched &sched;
  Display &display;
  Buzzer &buzzer;
  Button &button;
  IRtc &rtc;
  ITemp &temp;
  Alarm &alarm;
};

enum class PageType {
  CLOCK_MAIN_PAGE,
  CLOCK_SETUP_PAGE,
  ALARM_ALERT_PAGE,
  ALARM_VIEW_PAGE,
  ALARM_SETUP_PAGE,
  TEST_PAGE,
};

union PageBuf {
  PageBuf() {}
  ~PageBuf() {}
  ClockMainPage clockMainPage;
  ClockSetupPage clockSetupPage;
  AlarmAlertPage alarmAlertPage;
  AlarmViewPage alarmViewPage;
  AlarmSetupPage alarmSetupPage;
  TestPage testPage;
};

class PageManager {
public:
  PageManager(PageEnv &env, PageType startPageType);
  ~PageManager();
  void changePage(PageType nextPageType, uintptr_t arg = 0);

private:
  PageManager(const PageManager &) = delete;
  void operator=(const PageManager &) = delete;
  void nextPage();
  void createPage();
  void destoryPage();

  PageEnv &m_env;
  PageType m_currentPageType;
  PageType m_nextPageType;
  uintptr_t m_pageArg;
  PageBuf m_currentPage;
};