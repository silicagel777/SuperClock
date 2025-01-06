#pragma once

#include "driver/tone/itone.h"

class ToneTimer0 : public ITone {
public:
  explicit ToneTimer0();
  virtual void run(uint16_t freq) override;
  virtual void stop() override;

private:
  ToneTimer0(const ToneTimer0 &) = delete;
  void operator=(const ToneTimer0 &) = delete;
};
