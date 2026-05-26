#pragma once
#include <stdint.h>

namespace Timer {
// Warning: overrides Timer0. Cannot use _delay or Timer0 anymore.
void init();

// To maintain arduino-like API, all the timer functions are available in the
// global scope.
}  // namespace Timer

// Return the milliseconds elapsed since last reset
uint32_t millis();

// Return the microseconds elapsed since last reset. Due to the clock frequency,
// the highest resolution possible is 4us increments.
uint32_t micros();

// Delay execution for at least the specified ms. This is a busy loop and does
// not make the CPU sleep or be idle.
void delay(uint32_t ms);

// Delay execution for around the specified us. This is a busy loop and does not
// make the CPU sleep or be idle. To achieve us-level accuracy, the clock speed
// is used as a general timer and no-op instructions are used to introduce
// delays of scales around a microsecond.
void delayMicroseconds(uint32_t us);