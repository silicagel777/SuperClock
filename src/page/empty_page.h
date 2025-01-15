#pragma once

class EmptyPage {
public:
  EmptyPage() {};
  ~EmptyPage() {};

private:
  EmptyPage(const EmptyPage &) = delete;
  void operator=(const EmptyPage &) = delete;
};
