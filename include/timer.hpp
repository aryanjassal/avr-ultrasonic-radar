#pragma once
#include <stdint.h>

namespace Timer {
// Warning: overrides Timer 0. Cannot use _delay anymore.
void init();
uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);
} // namespace Timer

// Compatible with regular arduino millis
uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);