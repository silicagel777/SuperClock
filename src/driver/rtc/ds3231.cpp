#include <driver/i2c/i2c.h>

#include "ds3231.h"

Ds3231::Ds3231(I2C &i2c) : m_i2c(i2c) {}

uint8_t Ds3231::readTime(RtcTime *time) {
  uint8_t dataStart = 0;
  uint8_t data[7];
  uint8_t err;

  err = m_i2c.write(c_busAddr, &dataStart, sizeof(dataStart));
  if (err) {
    return err;
  }
  err = m_i2c.read(c_busAddr, data, sizeof(data));
  if (err) {
    return err;
  }

  time->second = ((data[0] >> 4) & 0x7) * 10 + (data[0] & 0xF);
  time->minute = ((data[1] >> 4) & 0x7) * 10 + (data[1] & 0xF);
  time->hour = ((data[2] >> 4) & 0x3) * 10 + (data[2] & 0xF);
  time->day = data[3] & 0x7;
  time->week = ((data[4] >> 4) & 0x3) * 10 + (data[4] & 0xF);
  time->month = ((data[5] >> 4) & 0x1) * 10 + (data[5] & 0xF);
  time->year = 2000 + ((data[6] >> 4) & 0x7) * 10 + (data[6] & 0xF);

  // TODO: prettier error handling
  // TODO: return result in bcd?

  return 0;
}

uint8_t Ds3231::readTemp(int16_t *temp) {
  uint8_t dataStart = 0x11;
  uint8_t data[2];
  uint8_t err;

  err = m_i2c.write(c_busAddr, &dataStart, sizeof(dataStart));
  if (err) {
    return err;
  }
  err = m_i2c.read(c_busAddr, data, sizeof(data));
  if (err) {
    return err;
  }

  *temp = (int16_t)data[0] * 100 + (data[1] >> 6) * 25;
  return 0;
}
