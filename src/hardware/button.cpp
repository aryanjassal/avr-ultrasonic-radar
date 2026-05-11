#include "hardware/button.hpp"
#include "drivers/timer.hpp"

Button::Button(InputPin& p) : pin(p) {};

bool Button::pressed() {
  bool state = pin.read();
  uint32_t now = millis();

  if (state != lastState) {
    lastChange = now;
    lastState = state;
  }

  if ((now - lastChange) > 30) {
    if (lastStable != state) {
      lastStable = state;

      // Falling edge: HIGH -> LOW
      if (!state) return true;
    }
  }

  return false;
}