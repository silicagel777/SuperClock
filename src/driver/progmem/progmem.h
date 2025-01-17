#pragma once

#include <avr/pgmspace.h>

// Annotate constants with PROGMEM and use the following stuff to read them

class Progmem {
public:
  Progmem() = delete;

  static inline void memcpy(void *dest, const void *src, size_t len) {
    memcpy_PF(dest, reinterpret_cast<uint_farptr_t>(src), len);
  }

  static inline uint8_t readU8(const void *src) {
    return pgm_read_byte(src);
  }

  static inline uint16_t readU16(const void *src) {
    return pgm_read_word(src);
  }
};
