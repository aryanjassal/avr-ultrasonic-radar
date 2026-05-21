#pragma once

#include "radar/controller.hpp"
#include "state.hpp"
#include "ui/screen.hpp"
#include "widgets/button.hpp"
#include "widgets/text.hpp"
#include "widgets/value.hpp"

static void onMinAngleSelect(Navigator* navigator) {
  state.servoPreview = ServoPreviewMode::MinAngle;
}

static void onMinAngleDeselect(Navigator* navigator) {
  state.servoPreview = ServoPreviewMode::None;
}

static void onMaxAngleSelect(Navigator* navigator) {
  state.servoPreview = ServoPreviewMode::MaxAngle;
}

static void onMaxAngleDeselect(Navigator* navigator) {
  state.servoPreview = ServoPreviewMode::None;
}

static void onNext(Navigator* navigator) {
  state.ready = true;
  RadarController::generateStoppingPoints();
  RadarController::start();
}

class CalibrationScreen : public Screen {
 private:
  // Widgets
  TextWidget title;
  TextWidget body;
  ValueWidget<uint8_t> minAngleInput;
  ValueWidget<uint8_t> maxAngleInput;
  ValueWidget<uint16_t> minDistanceInput;
  ValueWidget<uint16_t> maxDistanceInput;
  ButtonWidget nextButton;

 public:
  CalibrationScreen()
      : Screen(nullptr),
        title("CALIBRATE"),
        body(
            "Before using the system, please calibrate it. Click to edit a "
            "value. Click again to save. All angles are in degrees and "
            "distances in mm."),
        minAngleInput("MIN_ANGLE", &state.minAngle, 0, 80, onMinAngleSelect,
                      onMinAngleDeselect),
        maxAngleInput("MAX_ANGLE", &state.maxAngle, 100, 180, onMaxAngleSelect,
                      onMaxAngleDeselect),
        minDistanceInput("MIN_DIST", &state.minDistance, 0, 9999),
        maxDistanceInput("MAX_DIST", &state.maxDistance, 0, 10000),
        nextButton("NEXT", onNext) {
    addWidget(&title);
    addWidget(&body);
    addWidget(&minAngleInput);
    addWidget(&maxAngleInput);
    addWidget(&minDistanceInput);
    addWidget(&maxDistanceInput);
    addWidget(&nextButton);
  }

  void update(uint32_t now) {
    // Make sure invalid distances are not possible
    if (state.minDistance >= state.maxDistance) {
      state.minDistance = state.maxDistance - 1;
    }

    switch (state.servoPreview) {
      case (ServoPreviewMode::MaxAngle):
        RadarController::writeAngleToServo(state.maxAngle);
        break;
      case (ServoPreviewMode::MinAngle):
        RadarController::writeAngleToServo(state.minAngle);
        break;
      default: break;
    }
  }
};