#pragma once

#include "page/clock_page.h"
#include "page/demo_page.h"
#include "page/empty_page.h"

class Buzzer;
class Display;
class IRtc;
class ITemp;
class Sched;

class PageManager {
public:
  enum class PageType { EMPTY_PAGE, CLOCK_PAGE, DEMO_PAGE };

  PageManager(
      Sched &sched, Display &display, Buzzer &buzzer, Button &button, IRtc &rtc, ITemp &temp);
  void changePage(PageType nextPageType);

private:
  PageManager(const PageManager &) = delete;
  void operator=(const PageManager &) = delete;
  void createPage(PageType type);
  void destoryPage(PageType type);

  union AnyPage {
    EmptyPage emptyPage;
    ClockPage clockPage;
    DemoPage demoPage;
  };
  AnyPage m_currentPage{EmptyPage{}};
  PageType m_currentPageType = PageType::EMPTY_PAGE;

  Sched &m_sched;
  Display &m_display;
  Buzzer &m_buzzer;
  Button &m_button;
  IRtc &m_rtc;
  ITemp &m_temp;
};