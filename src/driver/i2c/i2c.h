#pragma once

#include <stdint.h>

class I2C {
public:
  enum class FreqMode { FREQ_100K, FREQ_250K, FREQ_400K };

  I2C(FreqMode freqMode, bool pullup = false);
  uint8_t write(uint8_t addr, uint8_t *data, uint8_t len, bool repeatStart = false);
  uint8_t read(uint8_t addr, uint8_t *data, uint8_t len);
  uint8_t writeReg(uint8_t addr, uint8_t reg, uint8_t data);
  uint8_t readReg(uint8_t addr, uint8_t reg, uint8_t *data);

private:
  I2C(const I2C &) = delete;
  void operator=(const I2C &) = delete;
  uint8_t transmitStart(void);
  static void transmitStop(void);
  uint8_t transmitAddr(uint8_t addr, bool readWrite);
  static uint8_t transmitByte(uint8_t data);
  static uint8_t receiveByte(uint8_t *data, bool readAck);
};
