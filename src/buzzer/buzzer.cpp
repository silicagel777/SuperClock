#include "buzzer.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Buzzer::Buzzer(Sched &sched, Tone &tone) : m_sched(sched), m_tone(tone) {}

void Buzzer::beep() {
  m_tone.run(1000);
  m_sched.addTask<Buzzer, &Buzzer::stop>(this, 100);
}

void Buzzer::stop() {
  m_tone.stop();
}
