#include "driver/display/display.h"
#include "driver/time/time.h"
#include "driver/tone/tone_timer0.h"
#include "sched/sched.h"
#include "system.h"

int main(void) {
  systemInit();

  Time time{};
  Sched sched{time};
  Display display{};
  ToneTimer0 buzzerTone{};

  buzzerTone.run(1000);
  sched.addTask([](void *p) { ((decltype(&buzzerTone))p)->stop(); }, &buzzerTone, 1000);
  for (;;) {
    sched.process();
    display.process();
  }
}