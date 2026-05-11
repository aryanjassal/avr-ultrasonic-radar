#pragma once

#include <stdint.h>

namespace Dial {
// The dial uses INT0 and INT1 hardware interrupts to determine dedent direction
// and thus is fixed to 1 per system.
void init();

// Returns accumulated detent movement
int8_t poll();

uint8_t pressed();
};  // namespace Dial