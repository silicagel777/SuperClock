#pragma once

#include <stdint.h>

class Adc {
public:
  static constexpr uint16_t c_maxValue = 1023;

  enum class ReferenceMode { AREF = 0, AVCC = 1, REF2V56 = 3 };
  enum class PrescalerMode {
    DIV2 = 1,
    DIV4 = 2,
    DIV8 = 3,
    DIV16 = 4,
    DIV32 = 5,
    DIV64 = 6,
    DIV128 = 7
  };

  Adc(ReferenceMode referenceMode, PrescalerMode prescalerMode);
  ~Adc();
  uint16_t read(uint8_t channel);

private:
  Adc(const Adc &) = delete;
  void operator=(const Adc &) = delete;
};
