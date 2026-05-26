#pragma once

#include <stdint.h>

enum class ServoPreviewMode { None, MinAngle, MaxAngle };

enum class RadarMode : uint8_t { Tracking, Alert };

struct AppState {
  // Calibration data
  uint8_t minAngle = 0;
  uint8_t maxAngle = 180;
  uint16_t minDistance = 20;
  uint16_t maxDistance = 200;
  uint8_t scanSpeed = 99;

  // To allow previewing the calibration angles
  ServoPreviewMode servoPreview = ServoPreviewMode::None;

  // If the calibration has been completed
  bool ready = false;

  // If the system is currently alarming
  bool alarmEnabled = false;
  uint16_t alarmThreshold = 210;

  // Mode
  RadarMode mode = RadarMode::Tracking;
  bool manualTrackingMode = false;

  // USART debugging
  bool sendAmplitude = false;
  bool sendAngle = false;
  bool sendDistance = false;
  bool usartRendering = false;
};

extern AppState state;
