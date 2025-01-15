#include "page/page_manager.h"
#include "sys/new.h"

PageManager::PageManager(PageEnv &env) : m_env(env) {}

PageManager::~PageManager() {
  destoryPage(m_currentPageType);
}

void PageManager::changePage(PageType nextPageType) {
  destoryPage(m_currentPageType);
  m_currentPageType = nextPageType;
  createPage(m_currentPageType);
}

void PageManager::createPage(PageType type) {
  switch (type) {
  case PageType::EMPTY_PAGE:
    new (&m_currentPage) EmptyPage{};
    break;
  case PageType::CLOCK_PAGE:
    new (&m_currentPage) ClockPage{*this, m_env};
    break;
  case PageType::DEMO_PAGE:
    new (&m_currentPage) DemoPage{*this, m_env};
    break;
  }
}

void PageManager::destoryPage(PageType type) {
  switch (type) {
  case PageType::EMPTY_PAGE:
    m_currentPage.emptyPage.~EmptyPage();
    break;
  case PageType::CLOCK_PAGE:
    m_currentPage.clockPage.~ClockPage();
    break;
  case PageType::DEMO_PAGE:
    m_currentPage.demoPage.~DemoPage();
    break;
  }
}
