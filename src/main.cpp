#include "driver/display/display.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

int main(void) {
  Time time{};
  Sched sched{time};
  Display display{};
  Tone buzzerTone{};

  buzzerTone.run(1000);
  sched.addTask([](void *p) { ((decltype(&buzzerTone))p)->stop(); }, &buzzerTone, 1000);
  for (;;) {
    sched.process();
    display.process();
  }
}
