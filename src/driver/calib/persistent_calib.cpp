#include "driver/calib/persistent_calib.h"
#include "driver/calib/icalib.h"
#include "driver/mem/eeprom.h"
#include "sched/sched.h"

static uint8_t EEMEM ge_calibEepromSignature;
static uint8_t EEMEM ge_calibValue;

PersistentCalib::PersistentCalib(Sched &sched, ICalib &calib) : m_sched(sched), m_calib(calib) {
  if (EepromMem::readU8(&ge_calibEepromSignature) != c_calibSignature) {
    // Rewrite EEPROM data if it's wrong
    int8_t runValue = 0;
    m_calib.readCalib(runValue);
    EepromMem::writeU8(&ge_calibValue, (uint8_t)runValue);
    EepromMem::writeU8(&ge_calibEepromSignature, c_calibSignature);
  }
  sync();
}

PersistentCalib::~PersistentCalib() {
  m_sched.removeTasks(this);
}

uint8_t PersistentCalib::setCalib(int8_t value) {
  EepromMem::writeU8(&ge_calibValue, (uint8_t)value);
  sync();
  return 0;
}

uint8_t PersistentCalib::readCalib(int8_t &value) {
  value = (int8_t)EepromMem::readU8(&ge_calibValue);
  return 0;
}

void PersistentCalib::sync() {
  if (!m_syncRunning) {
    retrySync();
  }
}

void PersistentCalib::retrySync() {
  m_syncRunning = true;
  int8_t savedValue = (int8_t)EepromMem::readU8(&ge_calibValue);
  int8_t runValue = 0;
  m_calib.readCalib(runValue);
  if (savedValue == runValue) {
    m_syncRunning = false;
    return;
  }
  m_calib.setCalib(savedValue);
  m_calib.readCalib(runValue);
  if (savedValue == runValue) {
    m_syncRunning = false;
    return;
  }
  // For some reason, DS3231 will only accept calibration after a few seconds
  // passed since boot. So, if we could not save it, let's retry in the background.
  m_sched.addTask<PersistentCalib, &PersistentCalib::retrySync>(this, c_syncDelay, 0);
}
