#include "buzzer/buzzer.h"
#include "buzzer/melody.h"
#include "driver/progmem/progmem.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Buzzer::Buzzer(Sched &sched, Tone &tone) : m_sched(sched), m_tone(tone) {}

Buzzer::~Buzzer() {
  stop();
}

void Buzzer::stop() {
  m_tone.stop();
  m_sched.removeTasks<Buzzer, &Buzzer::play>(this);
}

void Buzzer::beep() {
  stop();
  mp_note = cp_melodyBeep;
  mp_reload = nullptr;
  play();
}

void Buzzer::success() {
  stop();
  mp_note = cp_melodySuccess;
  mp_reload = nullptr;
  play();
}

void Buzzer::alarm() {
  stop();
  mp_note = cp_melodyAlarm;
  mp_reload = cp_melodyAlarm;
  play();
}

void Buzzer::play() {
  Note note;
  Progmem::memcpy(&note, mp_note, sizeof(Note));
  if (note.freq > 0) {
    m_tone.run(note.freq);
  } else {
    m_tone.stop();
  }
  if (note.delay > 0) {
    m_sched.addTask<Buzzer, &Buzzer::play>(this, note.delay);
    mp_note++;
  } else if (mp_reload) {
    m_sched.addTask<Buzzer, &Buzzer::play>(this, c_reloadDelay);
    mp_note = mp_reload;
  }
}