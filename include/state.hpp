#pragma once

#include <stdint.h>

enum class ServoPreviewMode { None, MinAngle, MaxAngle };

struct AppState {
  // Calibration data
  uint8_t minAngle = 0;
  uint8_t maxAngle = 180;
  uint16_t minDistance = 20;
  uint16_t maxDistance = 200;

  // To allow previewing the calibration angles
  ServoPreviewMode servoPreview = ServoPreviewMode::None;

  // If the calibration has been completed
  bool ready = false;

  // If the system is currently alarming
  bool alarmEnabled = false;
};

extern AppState state;