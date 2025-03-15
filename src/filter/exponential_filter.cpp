#include <filter/exponential_filter.h>

ExponentialFilter::ExponentialFilter(uint8_t alpha) : m_alpha(alpha) {}

void ExponentialFilter::reset(uint16_t initialValue) {
  m_result = initialValue;
}

uint16_t ExponentialFilter::filter(uint16_t input) {
  m_result = (input * m_alpha + m_result * (c_alphaLevels - m_alpha)) / c_alphaLevels;
  return m_result;
}
