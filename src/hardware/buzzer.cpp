#include "hardware/buzzer.hpp"

#include <avr/io.h>

Buzzer::Buzzer(const PinDescriptor& descriptor) : pin(descriptor) {
  TCCR2A = (1 << COM2A0) | (1 << WGM21);  // Timer2 CTC mode
  TCCR2B = (1 << CS22);                   // Prescaler 64
  stop();
}

void Buzzer::tone(uint16_t frequency) {
  if (frequency == 0) {
    stop();
    return;
  }

  // Toggle frequency:
  // f = F_CPU / (2 * N * (1 + OCR2A))
  uint32_t compare = (F_CPU / (2ul * 64ul * frequency)) - 1;
  if (compare > 255) compare = 255;
  OCR2A = (uint8_t)compare;
  TCNT2 = 0;
  TCCR2A = (1 << COM2A0) | (1 << WGM21);
  TCCR2B = (1 << CS22);
  enabled = true;
}

void Buzzer::stop() {
  // Normal port operation on OC2A
  TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));

  // Stop Timer2 clock
  TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));

  pin.low();
  enabled = false;
}

bool Buzzer::active() { return enabled; }