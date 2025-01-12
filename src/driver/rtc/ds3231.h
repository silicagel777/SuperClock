#pragma once

#include <driver/rtc/irtc.h>
#include <driver/temp/itemp.h>

class I2C;

class Ds3231 : public IRtc, public ITemp {
public:
  Ds3231(I2C &i2c);
  uint8_t readTime(RtcTime *time) override;
  uint8_t setTime(const RtcTime *time) override;
  uint8_t readTemp(int16_t *temp) override;

private:
  Ds3231(const Ds3231 &) = delete;
  void operator=(const Ds3231 &) = delete;
  static constexpr uint8_t c_busAddr = 0x68;
  I2C &m_i2c;
};
