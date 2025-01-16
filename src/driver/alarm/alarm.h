#pragma once

#include <stdint.h>

class Sched;
class IRtc;

class Alarm {
public:
  static constexpr uint8_t c_alarmCount = 5;
  static constexpr uint8_t c_alarmMax = c_alarmCount - 1;
  struct AlarmTime {
    bool enabled : 1;
    uint8_t hour : 7;
    uint8_t minute;
  };
  typedef void (*alarm_cb_t)(void *ctx);

  Alarm(Sched &sched, IRtc &rtc);
  void setCallback(alarm_cb_t cb, void *ctx);
  void resetCallback();
  uint8_t setAlarm(uint8_t num, const AlarmTime &time);
  uint8_t getAlarm(uint8_t num, AlarmTime &time);

  template <class C, void (C::*M)()>
  void setCallback(C *ctx) {
    auto cb = [](void *ctx) { (static_cast<C *>(ctx)->*M)(); };
    return setCallback(cb, ctx);
  }

private:
  Alarm(const Alarm &) = delete;
  void operator=(const Alarm &) = delete;
  void runCallback();
  void check();
  static constexpr uint8_t c_alarmSignature = 0x8D;
  static constexpr uint16_t c_checkDelay = 300;
  Sched &m_sched;
  IRtc &m_rtc;
  alarm_cb_t m_cb = nullptr;
  void *m_cbCtx = nullptr;
  AlarmTime m_alarms[c_alarmCount]{};
};
