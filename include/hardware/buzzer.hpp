#pragma once

#include "drivers/pin.hpp"

class Buzzer {
 private:
  OutputPin pin;
  bool enabled = false;

 public:
  // Use D11 (OC2A) ideally.
  Buzzer(const PinDescriptor& pin);

  // Start tone generation. Manual tone selection is lost due to needing Timer2
  // to be in Phase Correct TOP mode.
  void start();

  // Stop buzzer.
  void stop();

  // Is buzzer currently active?
  bool active();
};