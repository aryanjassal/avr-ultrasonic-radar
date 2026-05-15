#pragma once
#include <stdint.h>

namespace Timer {
// Warning: overrides Timer0. Cannot use _delay or Timer0 anymore.
void init();
uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);
}  // namespace Timer

// Compatible with regular arduino functions. However, accuracy might not be as
// high.
uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);