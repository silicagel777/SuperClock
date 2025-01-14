#include "page/page_manager.h"
#include "env/new.h"

PageManager::PageManager(
    Sched &sched, Display &display, Buzzer &buzzer, Button &button, IRtc &rtc, ITemp &temp)
    : m_sched(sched), m_display(display), m_buzzer(buzzer), m_button(button), m_rtc(rtc),
      m_temp(temp) {}

void PageManager::changePage(PageType nextPageType) {
  destoryPage(m_currentPageType);
  m_currentPageType = nextPageType;
  createPage(m_currentPageType);
}

void PageManager::createPage(PageType type) {
  switch (type) {
  case PageType::EMPTY_PAGE:
    new (&m_currentPage) EmptyPage{};
    m_currentPage.emptyPage.show();
    break;
  case PageType::CLOCK_PAGE:
    new (&m_currentPage) ClockPage{*this, m_sched, m_display, m_buzzer, m_button, m_rtc, m_temp};
    m_currentPage.clockPage.show();
    break;
  case PageType::DEMO_PAGE:
    new (&m_currentPage) DemoPage{*this, m_sched, m_display, m_buzzer, m_button};
    m_currentPage.demoPage.show();
    break;
  }
}

void PageManager::destoryPage(PageType type) {
  switch (type) {
  case PageType::EMPTY_PAGE:
    m_currentPage.emptyPage.hide();
    m_currentPage.emptyPage.~EmptyPage();
    break;
  case PageType::CLOCK_PAGE:
    m_currentPage.clockPage.hide();
    m_currentPage.clockPage.~ClockPage();
    break;
  case PageType::DEMO_PAGE:
    m_currentPage.demoPage.hide();
    m_currentPage.demoPage.~DemoPage();
    break;
  }
}
