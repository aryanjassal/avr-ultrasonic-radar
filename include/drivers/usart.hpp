#pragma once
#include <stdint.h>

namespace USART {
// Initialise USART with a given baud rate and if double speed should be used.
void init(uint32_t baud, bool doubleSpeed = false);

// Flush the internal RX buffer.
void flush();

// Prints a string to the screen. This function is also overloaded to allow
// printing single characters instead.
void print(const char* str);
void print(char c);

// Same as print, but appends a newline at the end. If called with no
// parameters, only the newline characters are printed.
void println();
void println(const char* str);

// Print integers.
void print_uint32(uint32_t num);
void print_int32(int32_t num);

// Print floats with a given maximum width (digits) and precision (decimals).
void print_float(float num, uint8_t width = 2, uint8_t precision = 2);

// Supply a custom callback to call when the RX interrupt is invoked. Do not
// call any other procedure which relies on interrupts, as no other interrupts
// can fire while inside an interrupt handler. Interrupt handlers must be lean
// to avoid starving other processes too.
void installRx(void (*handler)());
};  // namespace USART