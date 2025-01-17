#pragma once

#include <stdint.h>

class Sched;
class Tone;
struct Note;

class Buzzer {
public:
  Buzzer(Sched &sched, Tone &tone);
  void stop();
  void beep();
  void success();
  void alarm();

private:
  Buzzer(const Buzzer &) = delete;
  void operator=(const Buzzer &) = delete;
  void play();

  static constexpr uint16_t c_reloadDelay = 500;
  Sched &m_sched;
  Tone &m_tone;
  const Note *mp_note = nullptr;
  const Note *mp_reload = nullptr;
};
