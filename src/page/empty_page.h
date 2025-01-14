#pragma once

#include <page/ipage.h>

class EmptyPage : public IPage {
public:
  EmptyPage() {};
  void show() override {};
  void hide() override {};

private:
  EmptyPage(const EmptyPage &) = delete;
  void operator=(const EmptyPage &) = delete;
};
