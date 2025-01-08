#include "buzzer/buzzer.h"
#include "driver/display/display.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Time time{};
Sched sched{time};
Display display{};
Tone tone{};
Buzzer buzzer{sched, tone};

void animate(void *p) {
  static uint8_t animateVal = Display::c_maxPixelBrigtness - 1;
  static int8_t animateInc = 1;
  animateVal += animateInc;
  if (animateVal == 0 || animateVal == Display::c_maxPixelBrigtness) {
    animateInc = -animateInc;
  }
  display.writePixel(8, 1, animateVal);
  display.writePixel(8, 4, animateVal);
  sched.addTask(animate, nullptr, 30);
}

int main(void) {
  display.writeNumber(1, 0, 0);
  display.writeNumber(3, 4, 0);
  display.writeNumber(2, 10, 0);
  display.writeNumber(3, 14, 0);
  buzzer.beep();
  sched.addTask(animate, nullptr, 0);
  for (;;) {
    sched.process();
  }
}
