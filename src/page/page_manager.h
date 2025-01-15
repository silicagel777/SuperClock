#pragma once

#include "page/clock_page.h"
#include "page/demo_page.h"

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
  CLOCK_PAGE,
  DEMO_PAGE,
};

union PageBuf {
  PageBuf() {}
  ~PageBuf() {}
  ClockPage clockPage;
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
  void createPage();
  void destoryPage();

  PageEnv &m_env;
  PageType m_currentPageType;
  PageBuf m_currentPage;
};