#pragma once

#include <stdint.h>

struct AppState {
  uint8_t minAngle = 0;
  uint8_t maxAngle = 180;
  uint16_t minDistance = 20;
  uint16_t maxDistance = 200;
  bool alarmEnabled = false;
};

extern AppState state;