#include <avr/io.h>
#include <util/twi.h>

#include "i2c.h"

#define TW_PORT PORTC
#define TW_DDR DDRC
#define TW_PIN_SCL 0
#define TW_PIN_SDA 1

#define TW_READ_ACK 1
#define TW_READ_NACK 0

#define TW_ADDR_READ 0
#define TW_ADDR_WRITE 1

I2C::I2C(FreqMode freqMode, bool pullup) {
  TW_DDR |= (1 << TW_PIN_SDA) | (1 << TW_PIN_SCL);
  if (pullup) {
    TW_PORT |= (1 << TW_PIN_SDA) | (1 << TW_PIN_SCL);
  } else {
    TW_PORT &= ~((1 << TW_PIN_SDA) | (1 << TW_PIN_SCL));
  }
  TW_DDR &= ~((1 << TW_PIN_SDA) | (1 << TW_PIN_SCL));

  switch (freqMode) {
  case FreqMode::FREQ_100K:
    TWBR = ((F_CPU / 100000) - 16) / 2;
    break;
  case FreqMode::FREQ_250K:
    TWBR = ((F_CPU / 250000) - 16) / 2;
    break;
  case FreqMode::FREQ_400K:
    TWBR = ((F_CPU / 400000) - 16) / 2;
    break;
  }
}

uint8_t I2C::transmitStart(void) {
  /* Send START condition */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

  /* Wait for TWINT flag to be set */
  while (!(TWCR & (1 << TWINT))) {};

  /* Check status */
  if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START) {
    return TW_STATUS;
  }

  return 0;
}

void I2C::transmitStop(void) {
  /* Send STOP condition */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

uint8_t I2C::transmitAddr(uint8_t addr, bool readWrite) {
  /* Transmit peripheral address with read/write flag */
  TWDR = addr << 1 | readWrite;
  TWCR = (1 << TWINT) | (1 << TWEN);

  /* Wait for TWINT flag to set */
  while (!(TWCR & (1 << TWINT))) {};

  /* Check status */
  if (TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK) {
    return TW_STATUS;
  }
  return 0;
}

uint8_t I2C::transmitByte(uint8_t data) {
  /* Transmit 1 byte*/
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);

  /* Wait for TWINT flag to set */
  while (!(TWCR & (1 << TWINT))) {};

  if (TW_STATUS != TW_MT_DATA_ACK) {
    return TW_STATUS;
  }
  return 0;
}

uint8_t I2C::receiveByte(uint8_t *data, bool readAck) {
  if (readAck) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT))) {};
    if (TW_STATUS != TW_MR_DATA_ACK) {
      return TW_STATUS;
    }
  } else {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))) {};
    if (TW_STATUS != TW_MR_DATA_NACK) {
      return TW_STATUS;
    }
  }
  *data = TWDR;
  return 0;
}

uint8_t I2C::write(uint8_t addr, uint8_t *data, uint8_t len, bool repeatStart) {
  uint8_t err;
  err = transmitStart();
  if (err) {
    return err;
  }
  err = transmitAddr(addr, TW_WRITE);
  if (err) {
    return err;
  }
  for (int i = 0; i < len; ++i) {
    err = transmitByte(data[i]);
    if (err) {
      return err;
    }
  }
  if (!repeatStart) {
    transmitStop();
  }
  return 0;
}

uint8_t I2C::read(uint8_t addr, uint8_t *data, uint8_t len) {
  uint8_t err;
  err = transmitStart();
  if (err) {
    return err;
  }
  err = transmitAddr(addr, TW_READ);
  if (err) {
    return err;
  }
  for (int i = 0; i < len - 1; ++i) {
    err = receiveByte(&data[i], TW_READ_ACK);
    if (err) {
      return err;
    }
  }
  err = receiveByte(&data[len - 1], TW_READ_NACK);
  if (err) {
    return err;
  }
  transmitStop();
  return 0;
}

uint8_t I2C::writeReg(uint8_t addr, uint8_t reg, uint8_t data) {
  uint8_t result = 0;
  write(addr, &reg, 1, false);
  write(addr, &data, 1);
  return result;
}

uint8_t I2C::readReg(uint8_t addr, uint8_t reg) {
  uint8_t result = 0;
  write(addr, &reg, 1, false);
  read(addr, &result, 1);
  return result;
}
