#include "radar/controller.hpp"

#include "drivers/pin.hpp"
#include "drivers/timer.hpp"
#include "hardware/servo.hpp"
#include "hardware/ultrasonic.hpp"
#include "state.hpp"

static bool isOperating = false;
static uint8_t scanPoints[8];
static uint8_t pointCount = 0;
static int8_t currentPoint = 0;
static int8_t direction = 1;
static uint32_t waitUntilTime = 0;

void (*callback)(uint8_t angle, uint16_t distance, uint8_t pointIndex);

enum class ScanState { Moving, WaitingForServo, WaitingForEcho, Idle };

static ScanState scanState = ScanState::Moving;

void RadarController::generateStoppingPoints() {
  uint8_t min = state.minAngle;
  uint8_t max = state.maxAngle;

  // Ensure proper ordering
  if (max < min) {
    uint8_t t = min;
    min = max;
    max = t;
  }

  uint8_t range = max - min;

  // Target roughly one point every 30 degrees.
  // +2 because both endpoints are included.
  pointCount = (range / 30) + 2;

  // Clamp point count
  if (pointCount > 8) pointCount = 8;

  // Evenly distribute points across the range.
  // Endpoints are always guaranteed.
  for (uint8_t i = 0; i < pointCount; i++) {
    scanPoints[i] = min + ((uint32_t)i * range) / (pointCount - 1);
  }
}

void RadarController::init(const PinDescriptor& ultrasonicTrigger) {
  Servo::init();                        // Servo initialises Timer1
  Ultrasonic::init(ultrasonicTrigger);  // Ultrasonic adds its own configuration
}

void RadarController::setCallback(void (*_callback)(uint8_t angle,
                                                    uint16_t distance,
                                                    uint8_t pointIndex)) {
  callback = _callback;
}

void RadarController::update() {
  Servo::update();
  Ultrasonic::update();

  // Exit early if the system is not in operation
  if (!isOperating) return;

  switch (scanState) {
    case ScanState::Moving:
      Servo::write_angle(scanPoints[currentPoint]);
      scanState = ScanState::WaitingForServo;
      break;

    case ScanState::WaitingForServo:
      if (!Servo::moving()) {
        Ultrasonic::trigger();
        scanState = ScanState::WaitingForEcho;
      }
      break;

    case ScanState::WaitingForEcho:
      if (Ultrasonic::ready()) {
        uint16_t distance = Ultrasonic::distance_mm();
        uint8_t angle = scanPoints[currentPoint];

        // Execute the callback with the obtained angle and distance readings.
        callback(angle, distance, currentPoint);

        currentPoint += direction;

        if (currentPoint >= pointCount) {
          currentPoint = pointCount - 2;
          direction = -1;
        }
        if (currentPoint < 0) {
          currentPoint = 1;
          direction = 1;
        }

        if (currentPoint == 0) { direction = 1; }
        uint8_t delay = (99 - state.scanSpeed);
        if (delay > 0) {
          waitUntilTime = millis() + delay;
          scanState = ScanState::Idle;
        } else {
          scanState = ScanState::Moving;
        }
      }

      break;

    case ScanState::Idle:
      if (millis() > waitUntilTime) { scanState = ScanState::Moving; }
  }
}

void RadarController::stop() { isOperating = false; }

void RadarController::start() { isOperating = true; }

void RadarController::writeAngleToServo(uint8_t angle) {
  if (isOperating) return;
  Servo::write_angle(angle);
}

uint8_t* RadarController::getStoppingPoints() { return scanPoints; }

uint8_t RadarController::getStoppingPointCount() { return pointCount; }