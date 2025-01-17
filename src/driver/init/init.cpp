#include <avr/interrupt.h>

#include "driver/init/init.h"

Init::Init() {
  sei(); // We need interrupts for everything else to work
}

Init::~Init() {
  cli();
}
