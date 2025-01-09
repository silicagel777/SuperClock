#include <avr/io.h>

#include "adc.h"

Adc::Adc(ReferenceMode referenceMode, PrescalerMode prescalerMode) {
  // Set reference mode
  ADMUX |= (uint8_t)referenceMode << REFS0;
  // Set prescaler mode
  ADCSRA |= (1 << ADEN) | ((uint8_t)prescalerMode << ADPS0);
}

uint16_t Adc::read(uint8_t channel) {
  // Set channel
  ADMUX &= ~(MUX0 | MUX1 | MUX2 | MUX3 | MUX4);
  ADMUX |= channel << MUX0;
  // Start conversion
  ADCSRA |= 1 << ADSC;
  // Wait for conversion to complete
  while (ADCSRA & (1 << ADSC)) {}
  // Obtain result
  return ADC;
}
