#include "page/page_manager.h"
#include "sched/sched.h"
#include "util/new.h"

PageManager::PageManager(PageEnv &env, PageType startPageType)
    : m_env(env), m_currentPageType(startPageType) {
  createPage();
}

PageManager::~PageManager() {
  destoryPage();
}

void PageManager::changePage(PageType nextPageType) {
  m_nextPageType = nextPageType;
  // Run through scheduler to be sure that no page code is running
  m_env.sched.addTask<PageManager, &PageManager::nextPage>(this, 0);
}

void PageManager::nextPage() {
  destoryPage();
  m_currentPageType = m_nextPageType;
  createPage();
}

void PageManager::createPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_MAIN_PAGE:
    new (&m_currentPage) ClockMainPage{*this, m_env};
    break;
  case PageType::CLOCK_SETUP_PAGE:
    new (&m_currentPage) ClockSetupPage{*this, m_env};
    break;
  case PageType::ALARM_VIEW_PAGE:
    new (&m_currentPage) AlarmViewPage{*this, m_env};
    break;
  case PageType::DEMO_PAGE:
    new (&m_currentPage) DemoPage{*this, m_env};
    break;
  }
}

void PageManager::destoryPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_MAIN_PAGE:
    m_currentPage.clockMainPage.~ClockMainPage();
    break;
  case PageType::CLOCK_SETUP_PAGE:
    m_currentPage.clockSetupPage.~ClockSetupPage();
    break;
  case PageType::ALARM_VIEW_PAGE:
    m_currentPage.alarmViewPage.~AlarmViewPage();
    break;
  case PageType::DEMO_PAGE:
    m_currentPage.demoPage.~DemoPage();
    break;
  }
}
