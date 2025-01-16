#include <string.h>

#include "driver/rtc/buffered_rtc.h"
#include "sched/sched.h"

BufferedRtc::BufferedRtc(Sched &sched, IRtc &rtc) : m_sched(sched), m_rtc(rtc) {
  update();
}

uint8_t BufferedRtc::readTime(RtcTime *time) {
  *time = m_lastTime;
  return m_lastErr;
}

uint8_t BufferedRtc::setTime(const RtcTime *time) {
  uint8_t err = m_rtc.setTime(time);
  if (err == 0) {
    m_lastTime = *time;
    m_lastErr = 0;
  }
  return err;
}

void BufferedRtc::update() {
  m_lastErr = m_rtc.readTime(&m_lastTime);
  m_sched.addTask<BufferedRtc, &BufferedRtc::update>(this, c_updateDelay);
}
