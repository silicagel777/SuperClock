#pragma once

#include <stdint.h>

class IPage {
public:
  virtual void show() = 0;
  virtual void hide() = 0;

protected:
  ~IPage() = default;
};
