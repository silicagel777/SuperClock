#include "buzzer/buzzer.h"
#include "buzzer/melody.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Buzzer::Buzzer(Sched &sched, Tone &tone) : m_sched(sched), m_tone(tone) {}

void Buzzer::stop() {
  m_tone.stop();
  m_sched.removeTasks<Buzzer, &Buzzer::play>(this);
}

void Buzzer::beep() {
  stop();
  m_note = cp_melodyBeep;
  m_reload = nullptr;
  play();
}

void Buzzer::success() {
  stop();
  m_note = cp_melodySuccess;
  m_reload = nullptr;
  play();
}

void Buzzer::alarm() {
  stop();
  m_note = cp_melodyAlarm;
  m_reload = cp_melodyAlarm;
  play();
}

void Buzzer::play() {
  Note note;
  memcpy_PF(&note, (uint_farptr_t)m_note, sizeof(Note));
  if (note.freq > 0) {
    m_tone.run(note.freq);
  } else {
    m_tone.stop();
  }
  if (note.delay > 0) {
    m_sched.addTask<Buzzer, &Buzzer::play>(this, note.delay);
    m_note++;
  } else if (m_reload) {
    m_sched.addTask<Buzzer, &Buzzer::play>(this, c_reloadDelay);
    m_note = m_reload;
  }
}