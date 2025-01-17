#include "driver/init/init.h"
#include "init.h"

Init::Init() {
  sei(); // We need interrupts for everything else to work
}

Init::~Init() {
  cli();
}
