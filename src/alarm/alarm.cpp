#include "alarm/alarm.h"
#include "driver/mem/eeprom.h"
#include "driver/rtc/irtc.h"
#include "sched/sched.h"

static uint8_t EEMEM ge_alarmEepromSignature;
static uint8_t EEMEM ge_alarmEepromData[sizeof(Alarm::AlarmTime) * Alarm::c_alarmCount];

Alarm::Alarm(Sched &sched, IRtc &rtc) : m_sched(sched), m_rtc(rtc) {
  if (EepromMem::readU8(&ge_alarmEepromSignature) != c_alarmSignature) {
    // Clear EEPROM data if it's wrong
    EepromMem::writeBlock(m_alarms, &ge_alarmEepromData, sizeof(m_alarms));
    EepromMem::writeU8(&ge_alarmEepromSignature, c_alarmSignature);
  }
  EepromMem::readBlock(m_alarms, &ge_alarmEepromData, sizeof(m_alarms));
  m_sched.addTask<Alarm, &Alarm::check>(this, 0, c_checkDelay);
}

Alarm::~Alarm() {
  m_sched.removeTasks(this);
}

void Alarm::setCallback(alarm_cb_t cb, void *ctx) {
  m_cb = cb;
  m_cbCtx = ctx;
}

void Alarm::resetCallback() {
  m_cb = nullptr;
  m_cbCtx = nullptr;
}

void Alarm::runCallback() {
  if (m_cb) {
    m_cb(m_cbCtx);
  }
}

uint8_t Alarm::setAlarm(uint8_t num, const AlarmTime &time) {
  if (num >= c_alarmCount) {
    return 1;
  }
  m_alarms[num] = time;
  EepromMem::writeBlock(&m_alarms[num], &ge_alarmEepromData[num], sizeof(AlarmTime));
  return 0;
}

uint8_t Alarm::readAlarm(uint8_t num, AlarmTime &time) {
  if (num >= c_alarmCount) {
    return 1;
  }
  time = m_alarms[num];
  return 0;
}

void Alarm::check() {
  IRtc::RtcTime rtcTime{};
  m_rtc.readTime(rtcTime);
  for (uint8_t i = 0; i < c_alarmCount; i++) {
    AlarmTime alarmTime{};
    readAlarm(i, alarmTime);
    if (alarmTime.enabled && alarmTime.hour == rtcTime.hour && alarmTime.minute == rtcTime.minute) {
      alarmTime.enabled = false;
      setAlarm(i, alarmTime);
      runCallback();
    }
  }
}
