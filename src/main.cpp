#include "buzzer/buzzer.h"
#include "driver/display/display.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

static const uint8_t X = Display::c_maxPixelBrigtness;
static const uint8_t demoBmp[Display::c_height][Display::c_width] = {
    {0, 0, X, 0, X, X, X, 0, 0, 0, X, X, X, 0, X, X, X},
    {0, X, X, 0, 0, 0, X, 0, X, 0, 0, 0, X, 0, 0, 0, X},
    {0, 0, X, 0, X, X, X, 0, 0, 0, X, X, X, 0, X, X, X},
    {0, 0, X, 0, 0, 0, X, 0, 0, 0, X, 0, 0, 0, 0, 0, X},
    {0, 0, X, 0, X, X, X, 0, X, 0, X, X, X, 0, X, X, X},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, X, 0},
};

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
  display.writeBmp(0, 0, Display::c_width, Display::c_height, (const uint8_t *)demoBmp);
  buzzer.beep();
  sched.addTask(animate, nullptr, 0);
  for (;;) {
    sched.process();
  }
}
