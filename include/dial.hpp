#pragma once

#include <stdint.h>

class Dial {
public:
  static void init();

  static int8_t poll();

  static uint8_t pressed();
};