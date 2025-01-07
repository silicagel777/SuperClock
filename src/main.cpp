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
uint8_t dotVal = 0;
int8_t dotInc = 1;

void animate(void *p) {
  dotVal += dotInc;
  if (dotVal == 0 || dotVal == 15) {
    dotInc = -dotInc;
  }
  display.setDot(8, 1, dotVal);
  display.setDot(8, 2, dotVal);
  display.setDot(8, 3, dotVal);
  display.setDot(8, 4, dotVal);
  sched.addTask(animate, nullptr, 50);
}

int main(void) {
  buzzer.beep();
  animate(nullptr);
  for (;;) {
    sched.process();
  }
}
