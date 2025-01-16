#include <avr/eeprom.h>

#include "alarm.h"
#include "driver/alarm/alarm.h"
#include "driver/rtc/irtc.h"
#include "sched/sched.h"

static uint8_t EEMEM g_alarmEepromSignature;
static uint8_t EEMEM g_alarmEepromData[sizeof(Alarm::AlarmTime) * Alarm::c_alarmCount];

Alarm::Alarm(Sched &sched, IRtc &rtc) : m_sched(sched), m_rtc(rtc) {
  if (eeprom_read_byte(&g_alarmEepromSignature) != c_alarmSignature) {
    // Clear EEPROM data if it's wrong
    eeprom_update_block(m_alarms, &g_alarmEepromData, sizeof(m_alarms));
    eeprom_update_byte(&g_alarmEepromSignature, c_alarmSignature);
  }
  eeprom_read_block(m_alarms, &g_alarmEepromData, sizeof(m_alarms));
  m_sched.addTask<Alarm, &Alarm::check>(this, 0, c_checkDelay);
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
  eeprom_update_block(&m_alarms[num], &g_alarmEepromData[num], sizeof(AlarmTime));
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
