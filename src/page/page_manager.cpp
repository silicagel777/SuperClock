#include "page/page_manager.h"
#include "util/new.h"

PageManager::PageManager(PageEnv &env, PageType startPageType)
    : m_env(env), m_currentPageType(startPageType) {
  createPage();
}

PageManager::~PageManager() {
  destoryPage();
}

void PageManager::changePage(PageType nextPageType) {
  destoryPage();
  m_currentPageType = nextPageType;
  createPage();
}

void PageManager::createPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_PAGE:
    new (&m_currentPage) ClockPage{*this, m_env};
    break;
  case PageType::DEMO_PAGE:
    new (&m_currentPage) DemoPage{*this, m_env};
    break;
  }
}

void PageManager::destoryPage() {
  switch (m_currentPageType) {
  case PageType::CLOCK_PAGE:
    m_currentPage.clockPage.~ClockPage();
    break;
  case PageType::DEMO_PAGE:
    m_currentPage.demoPage.~DemoPage();
    break;
  }
}
