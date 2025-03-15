#include <filter/exponential_filter.h>

static constexpr float PI_F = 3.1415927f;

ExponentialFilter::ExponentialFilter(float cutoffFrequency, float deltaT) {
  float rc = 1.0f / (2.0f * PI_F * cutoffFrequency);
  m_alpha = deltaT / (rc + deltaT) * c_alphaLevels;
}

void ExponentialFilter::reset(uint16_t initialValue) {
  m_result = initialValue;
}

uint16_t ExponentialFilter::filter(uint16_t input) {
  m_result = (input * m_alpha + m_result * (c_alphaLevels - m_alpha)) / c_alphaLevels;
  return m_result;
}
