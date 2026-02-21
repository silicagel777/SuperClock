#pragma once

#include <driver/calib/icalib.h>

class Sched;
class ICalib;

// A wrapper for calibration driver that remembers set value in EEPROM
// This way, calibration value is preserved when RTC battery is replaced
class PersistentCalib : public ICalib {
public:
  PersistentCalib(Sched &sched, ICalib &calib);
  ~PersistentCalib();
  uint8_t setCalib(int8_t value) override;
  uint8_t readCalib(int8_t &value) override;

private:
  PersistentCalib(const PersistentCalib &) = delete;
  void operator=(const PersistentCalib &) = delete;
  void sync();
  void retrySync();
  static constexpr uint8_t c_calibSignature = 0x8D;
  static constexpr uint16_t c_syncDelay = 100;
  Sched &m_sched;
  ICalib &m_calib;
  bool m_syncRunning = false;
};
