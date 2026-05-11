#include "pwm.hpp"
#include <avr/io.h>

void PWM::init() {
  // Timer1: fast PWM 8-bit, non-inverting
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
  TCCR1B |= (1 << WGM12) | (1 << CS11);

  // Timer2: fast PWM, non-inverting
  TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B |= (1 << CS21);
}