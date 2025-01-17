#pragma once

#include <driver/rtc/irtc.h>
#include <driver/temp/itemp.h>

class Sched;

class BufferedRtc : public IRtc {
public:
  static constexpr uint16_t c_updateDelay = 500;
  BufferedRtc(Sched &sched, IRtc &rtc);
  ~BufferedRtc();
  uint8_t readTime(RtcTime &time) override;
  uint8_t setTime(const RtcTime &time) override;

private:
  BufferedRtc(const BufferedRtc &) = delete;
  void operator=(const BufferedRtc &) = delete;
  void update();
  Sched &m_sched;
  IRtc &m_rtc;
  IRtc::RtcTime m_lastTime{};
  uint8_t m_lastErr = 0;
};
