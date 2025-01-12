#include <driver/i2c/i2c.h>

#include "ds3231.h"

Ds3231::Ds3231(I2C &i2c) : m_i2c(i2c) {}

uint8_t Ds3231::readTime(RtcTime *time) {
  uint8_t data;
  uint8_t err;

  err = m_i2c.readReg(c_busAddr, 0, &data);
  if (err) {
    return err;
  }
  time->sec = ((data >> 4) & 0x7) * 10 + (data & 0xF);

  err = m_i2c.readReg(c_busAddr, 1, &data);
  if (err) {
    return err;
  }
  time->min = ((data >> 4) & 0x7) * 10 + (data & 0xF);

  err = m_i2c.readReg(c_busAddr, 2, &data);
  if (err) {
    return err;
  }
  time->hr = ((data >> 4) & 0x3) * 10 + (data & 0xF);

  err = m_i2c.readReg(c_busAddr, 3, &data);
  if (err) {
    return err;
  }
  time->day = data & 0x7;

  err = m_i2c.readReg(c_busAddr, 4, &data);
  if (err) {
    return err;
  }
  time->date = ((data >> 4) & 0x3) * 10 + (data & 0xF);

  err = m_i2c.readReg(c_busAddr, 5, &data);
  if (err) {
    return err;
  }
  time->mon = ((data >> 4) & 0x1) * 10 + (data & 0xF);

  err = m_i2c.readReg(c_busAddr, 6, &data);
  if (err) {
    return err;
  }
  time->yr = 2000 + ((data >> 4) & 0x7) * 10 + (data & 0xF);

  // TODO: prettier error handling
  // TODO: return result in bcd?

  return 0;
}