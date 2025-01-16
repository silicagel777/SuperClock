#include <driver/i2c/i2c.h>

#include "ds3231.h"

static inline uint8_t decodeBcd(uint8_t num) {
  return (num >> 4) * 10 + (num & 0x0F);
}

static inline uint8_t encodeBcd(uint8_t num) {
  return (num / 10) << 4 | (num % 10);
}

Ds3231::Ds3231(I2C &i2c) : m_i2c(i2c) {}

uint8_t Ds3231::readTime(RtcTime &time) {
  const uint8_t dataStart = 0;
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

  time.second = decodeBcd(data[0] & 0b01111111);
  time.minute = decodeBcd(data[1] & 0b01111111);
  time.hour = decodeBcd(data[2] & 0b00111111);
  time.week = decodeBcd(data[3] & 0b00000111);
  time.day = decodeBcd(data[4] & 0b00111111);
  time.month = decodeBcd(data[5] & 0b00011111);
  time.year = 2000 + decodeBcd(data[6] & 0b11111111);

  // TODO: prettier error handling
  // TODO: return result in bcd?

  return 0;
}

uint8_t Ds3231::setTime(const RtcTime &time) {
  constexpr uint8_t dataStart = 0;
  uint8_t data[8];
  uint8_t err;

  data[0] = dataStart;
  data[1] = encodeBcd(time.second) & 0b01111111;
  data[2] = encodeBcd(time.minute) & 0b01111111;
  data[3] = encodeBcd(time.hour) & 0b00111111;
  data[4] = encodeBcd(time.week) & 0b00000111;
  data[5] = encodeBcd(time.day) & 0b00111111;
  data[6] = encodeBcd(time.month) & 0b00011111;
  data[7] = encodeBcd(time.year - 2000) & 0b11111111;

  err = m_i2c.write(c_busAddr, data, sizeof(data));
  if (err) {
    return err;
  }

  return 0;
}

uint8_t Ds3231::readTemp(int16_t &temp) {
  const uint8_t dataStart = 0x11;
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

  temp = (int16_t)data[0] * 100 + (data[1] >> 6) * 25;
  return 0;
}
