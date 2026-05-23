#pragma once

#include "drivers/usart.hpp"
#include "radar/controller.hpp"
#include "radar/display.hpp"
#include "state.hpp"
#include "ui/screen.hpp"
#include "widgets/button.hpp"
#include "widgets/popup.hpp"
#include "widgets/text.hpp"
#include "widgets/value.hpp"

static inline void printCalibrationMetrics() {
  // Min distance
  USART::print("$mind:");
  USART::print_int32(state.minDistance);
  USART::println();
  // Max distance
  USART::print("$maxd:");
  USART::print_int32(state.maxDistance);
  USART::println();
  // Min angle
  USART::print("$mina:");
  USART::print_int32(state.minAngle);
  USART::println();
  // Max angle
  USART::print("$maxa:");
  USART::print_int32(state.maxAngle);
  USART::println();
}

class CalibrationScreen : public Screen {
 private:
  // Widgets
  TextWidget title;
  ValueWidget<uint8_t> minAngleInput;
  ValueWidget<uint8_t> maxAngleInput;
  ValueWidget<uint16_t> minDistanceInput;
  ValueWidget<uint16_t> maxDistanceInput;
  ValueWidget<uint8_t> speedInput;
  ValueWidget<uint16_t> alarmThresholdInput;
  PopupWidget<1> helpPopup;
  TextWidget helpBody;
  ButtonWidget nextButton;

  Widget* helpPopupBody[1] = {&helpBody};

  // Callbacks
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
    if (state.usartRendering) printCalibrationMetrics();
    navigator->navigate(ScreenID::Menu);
  }

 public:
  CalibrationScreen()
      : Screen(ScreenID::Calibration, ScreenID::None),
        title("CALIBRATE"),
        minAngleInput("MIN_ANGLE", &state.minAngle, 0, 80, onMinAngleSelect,
                      onMinAngleDeselect),
        maxAngleInput("MAX_ANGLE", &state.maxAngle, 100, 180, onMaxAngleSelect,
                      onMaxAngleDeselect),
        minDistanceInput("MIN_DIST", &state.minDistance, 0, 9999),
        maxDistanceInput("MAX_DIST", &state.maxDistance, 0, 10000),
        speedInput("SPEED", &state.scanSpeed, 1, 99),
        alarmThresholdInput("ALARM_THRESHOLD", &state.alarmThreshold, 100, 300),
        helpPopup(id, "HELP", "HELP", helpPopupBody),
        helpBody(
            "Click to edit a value. Click again to save. All angles are in "
            "degrees and distances in mm."),
        nextButton("NEXT", onNext) {
    addWidget(&title);
    addWidget(&minAngleInput);
    addWidget(&maxAngleInput);
    addWidget(&minDistanceInput);
    addWidget(&maxDistanceInput);
    addWidget(&speedInput);
    addWidget(&alarmThresholdInput);
    addWidget(&helpPopup);
    addWidget(&nextButton);
  }

  void onEnter() override { RadarDisplay::clear(); }

  void update(uint32_t now) override {
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
