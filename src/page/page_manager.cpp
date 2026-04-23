#include "page/page_manager.h"
#include "sched/sched.h"
#include "util/new.h"

PageManager::PageManager(PageEnv &env, PageType startPageType, uintptr_t startPageArg)
    : m_env(env), m_currentPageType(startPageType), m_pageArg(startPageArg) {
  createPage();
}

PageManager::~PageManager() {
  destroyPage();
}

void PageManager::changePage(PageType nextPageType, uintptr_t arg) {
  m_nextPageType = nextPageType;
  m_pageArg = arg;
  // Run through scheduler to be sure that no page code is running
  m_env.sched.addTask<PageManager, &PageManager::nextPage>(this, 0);
}

void PageManager::nextPage() {
  destroyPage();
  m_currentPageType = m_nextPageType;
  createPage();
}

void PageManager::createPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_MAIN_PAGE:
    new (&m_currentPage) ClockMainPage{*this, m_env, m_pageArg};
    break;
  case PageType::CLOCK_SETUP_PAGE:
    new (&m_currentPage) ClockSetupPage{*this, m_env, m_pageArg};
    break;
  case PageType::ALARM_ALERT_PAGE:
    new (&m_currentPage) AlarmAlertPage{*this, m_env, m_pageArg};
    break;
  case PageType::ALARM_VIEW_PAGE:
    new (&m_currentPage) AlarmViewPage{*this, m_env, m_pageArg};
    break;
  case PageType::ALARM_SETUP_PAGE:
    new (&m_currentPage) AlarmSetupPage{*this, m_env, m_pageArg};
    break;
  case PageType::STOPWATCH_PAGE:
    new (&m_currentPage) StopWatchPage{*this, m_env, m_pageArg};
    break;
  case PageType::TEST_PAGE:
    new (&m_currentPage) TestPage{*this, m_env, m_pageArg};
    break;
  }
}

void PageManager::destroyPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_MAIN_PAGE:
    m_currentPage.clockMainPage.~ClockMainPage();
    break;
  case PageType::CLOCK_SETUP_PAGE:
    m_currentPage.clockSetupPage.~ClockSetupPage();
    break;
  case PageType::ALARM_ALERT_PAGE:
    m_currentPage.alarmAlertPage.~AlarmAlertPage();
    break;
  case PageType::ALARM_VIEW_PAGE:
    m_currentPage.alarmViewPage.~AlarmViewPage();
    break;
  case PageType::ALARM_SETUP_PAGE:
    m_currentPage.alarmSetupPage.~AlarmSetupPage();
    break;
  case PageType::STOPWATCH_PAGE:
    m_currentPage.stopwatchPage.~StopWatchPage();
    break;
  case PageType::TEST_PAGE:
    m_currentPage.testPage.~TestPage();
    break;
  }
}
