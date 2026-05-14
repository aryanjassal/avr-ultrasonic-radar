#pragma once

#include <stdint.h>

namespace Dial {
// The dial uses INT0 and INT1 hardware interrupts to determine dedent direction
// and thus is fixed to 1 per system.
void init();

// Returns accumulated detent movement
int8_t poll();

// Pressed returns the state of the button. To detect clicks, where only the
// rising edge is detected and debounced, use the `clicked()` method.
uint8_t pressed();

// Returns if the button has been clicked or not. Debounces and fires once
// during the rising edge. To read raw switch state, use the `pressed()` method.
uint8_t clicked();
};  // namespace Dial