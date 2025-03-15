#pragma once

#include <filter/ifilter.h>
#include <stdint.h>

class ExponentialFilter : public IFilter {
public:
  ExponentialFilter(float cutoffFrequency, float deltaT);
  virtual void reset(uint16_t initialValue);
  virtual uint16_t filter(uint16_t input);

private:
  static constexpr uint16_t c_alphaLevels = 256;
  uint16_t m_result = 0;
  uint8_t m_alpha;
};
