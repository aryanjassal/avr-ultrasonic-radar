#pragma once

#include <stdint.h>

namespace Dial {
// The dial uses INT0 and INT1 hardware interrupts to determine dedent direction
// and thus is fixed to 1 per system.
void init();

// Returns accumulated detent movement. The dial stores movement data. As such,
// if the dial state is not read, then it can store movement and returns it when
// it *is* read. This can cause issues when the dial movement is being
// purposefully ignored. In such cases, call `Dial::reset()` to clear stored
// movement data.
int8_t poll();

// Resets accumulated movement.
void reset();

// Pressed returns the state of the button. To detect clicks, where only the
// rising edge is detected and debounced, use the `clicked()` method.
uint8_t pressed();

// Returns if the button has been clicked or not. Debounces and fires once
// during the rising edge. To read raw switch state, use the `pressed()` method.
uint8_t clicked();
};  // namespace Dial