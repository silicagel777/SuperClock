#pragma once

#include <stdint.h>

class Sched;
class Tone;

class Buzzer {
public:
  Buzzer(Sched &sched, Tone &tone);
  void beep();
  void stop();

private:
  Buzzer(const Buzzer &) = delete;
  void operator=(const Buzzer &) = delete;

  Sched &m_sched;
  Tone &m_tone;
};
