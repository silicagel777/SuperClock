#pragma once

#include "page/clock/clock_main_page.h"
#include "page/clock/clock_setup_page.h"
#include "page/demo/demo_page.h"

class Sched;
class Display;
class Buzzer;
class Button;
class IRtc;
class ITemp;

struct PageEnv {
  Sched &sched;
  Display &display;
  Buzzer &buzzer;
  Button &button;
  IRtc &rtc;
  ITemp &temp;
};

enum class PageType {
  CLOCK_MAIN_PAGE,
  CLOCK_SETUP_PAGE,
  DEMO_PAGE,
};

union PageBuf {
  PageBuf() {}
  ~PageBuf() {}
  ClockMainPage clockMainPage;
  ClockSetupPage clockSetupPage;
  DemoPage demoPage;
};

class PageManager {
public:
  PageManager(PageEnv &env, PageType startPageType);
  ~PageManager();
  void changePage(PageType nextPageType);

private:
  PageManager(const PageManager &) = delete;
  void operator=(const PageManager &) = delete;
  void nextPage();
  void createPage();
  void destoryPage();

  PageEnv &m_env;
  PageType m_currentPageType;
  PageType m_nextPageType;
  PageBuf m_currentPage;
};