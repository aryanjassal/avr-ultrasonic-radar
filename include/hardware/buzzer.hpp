#pragma once

#include <stdint.h>

#include "drivers/pin.hpp"

class Buzzer {
 private:
  OutputPin pin;
  bool enabled = false;

 public:
  // Use D11 (OC2A) ideally.
  Buzzer(const PinDescriptor& pin);

  // Start tone generation.
  void tone(uint16_t frequency);

  // Stop buzzer.
  void stop();

  // Is buzzer currently active?
  bool active();
};