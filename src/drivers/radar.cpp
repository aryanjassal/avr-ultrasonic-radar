#include "drivers/radar.hpp"

#include "drivers/pin.hpp"
#include "hardware/servo.hpp"
#include "hardware/ultrasonic.hpp"
#include "state.hpp"

static bool isOperating = false;
static uint8_t scanPoints[8];
static uint8_t pointCount = 0;
static int8_t currentPoint = 0;
static int8_t direction = 1;

void (*callback)(uint8_t angle, uint16_t distance);

enum class ScanState { Moving, WaitingForServo, WaitingForEcho };

static ScanState scanState = ScanState::Moving;

void RadarController::generateStoppingPoints() {
  uint8_t min = state.minAngle;
  uint8_t max = state.maxAngle;

  if (max < min) {
    uint8_t t = min;
    min = max;
    max = t;
  }

  uint8_t range = max - min;
  pointCount = 0;

  // ~30 degree spacing
  uint8_t step = range / 7;
  if (step < 30) { step = 30; }
  if (step == 0) { step = 1; }

  for (uint8_t a = min; a < max; a += step) {
    scanPoints[pointCount++] = a;
    if (pointCount >= 7) break;
  }

  // Always ensure max included
  scanPoints[pointCount++] = max;

  // Ensure minimum 2 points
  if (pointCount < 2) {
    scanPoints[0] = min;
    scanPoints[1] = max;
    pointCount = 2;
  }
}

void RadarController::init(const PinDescriptor& ultrasonicTrigger) {
  Servo::init();                        // Servo initialises Timer1
  Ultrasonic::init(ultrasonicTrigger);  // Ultrasonic adds its own configuration
}

void RadarController::setCallback(void (*_callback)(uint8_t angle,
                                                    uint16_t distance)) {
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
        callback(angle, distance);

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
        scanState = ScanState::Moving;
      }
      break;
  }
}

void RadarController::stop() { isOperating = false; }

void RadarController::start() { isOperating = true; }

void RadarController::writeAngleToServo(uint8_t angle) {
  if (isOperating) return;
  Servo::write_angle(angle);
}