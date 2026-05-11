#pragma once
#include "pin.hpp"
#include <stdint.h>

class Buzzer {
  OutputPin pin;
  uint32_t stopTime = 0;
  uint32_t period = 0;
  uint32_t highTime = 0;
  uint32_t lastEdge = 0;
  bool level = false;

public:
  Buzzer(const PinDescriptor &d);
  void update();
  void play(uint16_t freq, uint8_t duty, uint16_t duration_ms);
  void stop();
};