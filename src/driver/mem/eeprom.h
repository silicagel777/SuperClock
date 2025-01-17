#pragma once

#include <avr/eeprom.h>

// Annotate variables with EEMEM and use the following stuff to read/write them

class EepromMem {
public:
  EepromMem() = delete;

  static inline void readBlock(void *dest, const void *src, size_t len) {
    eeprom_read_block(dest, src, len);
  }

  static inline void writeBlock(const void *src, void *dest, size_t len) {
    eeprom_update_block(src, dest, len);
  }

  static inline uint8_t readU8(const uint8_t *src) {
    return eeprom_read_byte(src);
  }

  static inline void writeU8(uint8_t *dest, uint8_t value) {
    eeprom_update_byte(dest, value);
  }

  static inline uint16_t readU16(const uint16_t *src) {
    return eeprom_read_word(src);
  }

  static inline void writeU16(uint8_t *dest, uint8_t value) {
    eeprom_update_byte(dest, value);
  }
};
