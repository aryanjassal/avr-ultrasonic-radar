#include "adc.hpp"
#include <avr/io.h>

void Adc::init() {
  // AVcc reference
  ADMUX = (1 << REFS0);
  // Enable ADC, prescaler 128
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}