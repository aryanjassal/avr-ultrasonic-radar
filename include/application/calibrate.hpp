#pragma once

#include "ui/screen.hpp"
#include "widgets/text.hpp"
#include "widgets/value.hpp"

class CalibrationScreen : public Screen {
 private:
  // Data
  uint8_t minAngle = 0;
  uint8_t maxAngle = 180;

  // Widgets
  TextWidget title;
  TextWidget body;
  ValueWidget<uint8_t> minAngleInput;
  ValueWidget<uint8_t> maxAngleInput;

 public:
  CalibrationScreen()
      : title("CALIBRATE"),
        body(
            "Before using the system, please calibrate it. Click to edit "
            "value, click again to save. Next to continue. Angles are in "
            "degrees, distances in mm."),
        minAngleInput("MIN_ANGLE", &minAngle, 0, 80),
        maxAngleInput("MAX_ANGLE", &maxAngle, 100, 180) {
    addWidget(&title);
    addWidget(&body);
    addWidget(&minAngleInput);
    addWidget(&maxAngleInput);
  }
};