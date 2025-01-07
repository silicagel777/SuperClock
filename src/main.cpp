#include "driver/display/display.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "buzzer/buzzer.h"
#include "sched/sched.h"

Time time{};
Sched sched{time};
Display display{};
Tone tone{};
Buzzer buzzer{sched, tone};
uint8_t animateVal = 0;
int8_t animateInc = 1;

void animate(void *p) {
  animateVal += animateInc;
  if (animateVal == 0 || animateVal == 255) {
    animateInc = -animateInc;
  }
  display.setBrigntess(animateVal);
  sched.addTask(animate, nullptr, 10);
}

int main(void) {
  buzzer.beep();
  animate(nullptr);
  for (;;) {
    sched.process();
  }
}
