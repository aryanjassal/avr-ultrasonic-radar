#pragma once
#include <stdint.h>

class USART {
public:
  static void init(uint32_t baud);
  static void flush();
  static void print(const char *str);
  static void print(char c);
  static void println();
  static void println(const char *str);
  static void print_uint32(uint32_t num);
  static void print_int32(int32_t num);
  static void print_float(float num, uint8_t width = 2, uint8_t precision = 2);
  static void installRx(void (*handler)());
};