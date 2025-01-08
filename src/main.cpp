#include "buzzer/buzzer.h"
#include "driver/display/display.h"
#include "driver/i2c/i2c.h"
#include "driver/time/time.h"
#include "driver/tone/tone.h"
#include "sched/sched.h"

Time time{};
Sched sched{time};
Display display{};
Tone tone{};
Buzzer buzzer{sched, tone};
I2C i2c{I2C::FreqMode::FREQ_400K};

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

void update(void *p) {
  uint8_t bcdSeconds = i2c.readReg(0x68, 0);
  uint8_t seconds = ((bcdSeconds >> 4) & 0x7) * 10 + (bcdSeconds & 0xF);
  uint8_t dotPos = seconds / 4 + 1;
  for (uint8_t i = 0; i < Display::c_width; i++) {
    display.writePixel(i, 5, 0);
  }
  display.writePixel(dotPos, 5);

  uint8_t bcdMinutes = i2c.readReg(0x68, 1);
  display.writeNumber((bcdMinutes >> 4) & 0x7, 10, 0);
  display.writeNumber(bcdMinutes & 0xF, 14, 0);

  uint8_t bcdHours = i2c.readReg(0x68, 2);
  display.writeNumber((bcdHours >> 4) & 0x3, 0, 0);
  display.writeNumber(bcdHours & 0xF, 4, 0);

  sched.addTask(update, nullptr, 500);
}

int main(void) {
  buzzer.beep();
  sched.addTask(animate, nullptr, 0);
  sched.addTask(update, nullptr, 0);
  for (;;) {
    sched.process();
  }
}
