#pragma once

#include <stdint.h>

#include "drivers/pin.hpp"

class Ultrasonic {
  OutputPin trig;
  InputPin echo;

  uint32_t lastPing = 0;
  uint16_t lastDistance = 0;
  bool measurementReady = false;

 public:
  Ultrasonic(const PinDescriptor& trigPin, const PinDescriptor& echoPin);

  void trigger();  // Start measurement
  bool ready() const;
  uint16_t distance_mm() const;
};