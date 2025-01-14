#include "page/page_manager.h"
#include "sys/new.h"

PageManager::PageManager(PageEnv &env) : m_env(env) {}

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
    new (&m_currentPage) ClockPage{*this, m_env};
    m_currentPage.clockPage.show();
    break;
  case PageType::DEMO_PAGE:
    new (&m_currentPage) DemoPage{*this, m_env};
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
