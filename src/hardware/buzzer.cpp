#include "hardware/buzzer.hpp"

#include <avr/io.h>

Buzzer::Buzzer(const PinDescriptor& descriptor) : pin(descriptor) { stop(); }

void Buzzer::start() {
  // Timer2 Phase Correct PWM (Top=0xff) (this gotta qualify for Phase Correct
  // TOP mode. surely.)
  TCCR2A = (1 << COM2A1) | (1 << WGM20);
  TCCR2B |= (1 << CS21);  // ~3.9 kHz
  OCR2A = 127;            // 50% duty
  enabled = true;
}

void Buzzer::stop() {
  TCCR2A &= ~((1 << COM2A1) | (1 << COM2A0));            // Disconnect OC2A
  TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));  // Stop timer
  pin.low();
  enabled = false;
}

bool Buzzer::active() { return enabled; }