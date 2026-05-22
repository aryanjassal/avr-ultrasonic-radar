#pragma once

#include <stdint.h>

#include "drivers/pin.hpp"

class Sound {
 private:
  AdcPin input;
  uint16_t peakToPeak = 0;
  uint16_t signalMax = 0;
  uint16_t signalMin = 1023;
  uint32_t lastSampleMicros = 0;
  uint32_t windowStartMillis = 0;
  bool spikeDetected = false;

 public:
  Sound(const PinDescriptor& input);

  // Update sampler state. Must be called continuously in loop.
  void update();

  // Latest peak-to-peak amplitude.
  uint16_t amplitude();

  // Was a spike recently detected?
  bool spike();

  // Clear spike flag manually.
  void clearSpike();
};