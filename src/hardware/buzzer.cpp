#include "hardware/buzzer.hpp"

#include <avr/io.h>

#include "drivers/pin.hpp"
#include "drivers/timer.hpp"

Buzzer::Buzzer(const PinDescriptor& d) : pin(d) {}

void Buzzer::play(uint16_t freq, uint8_t duty, uint16_t duration_ms) {
  if (stopTime != 0) return;
  uint32_t p = 1000000ul / freq;

  period = p;
  highTime = (p * duty) / 100;
  stopTime = millis() + duration_ms;
  lastEdge = micros();
  level = true;

  pin.high();
}

void Buzzer::update() {
  if (stopTime == 0) return;

  uint32_t now = micros();
  uint32_t elapsed = now - lastEdge;
  uint32_t edgeTime = level ? highTime : (period - highTime);

  if (elapsed >= edgeTime) {
    level = !level;
    lastEdge = now;

    if (level)
      pin.high();
    else
      pin.low();
  }

  if (millis() >= stopTime) {
    pin.low();
    stopTime = 0;
  }
}