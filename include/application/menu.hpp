#pragma once

#include "drivers/matrix.hpp"
#include "radar/controller.hpp"
#include "radar/display.hpp"
#include "state.hpp"
#include "ui/screen.hpp"
#include "widgets/button.hpp"
#include "widgets/multiselect.hpp"
#include "widgets/text.hpp"

static void onStop(Navigator* navigator) { RadarController::stop(); }

static void onStart(Navigator* navigator) { RadarController::start(); }

static void onRecalibrate(Navigator* navigator) {
  RadarController::stop();
  state.ready = false;
  navigator->navigate(ScreenID::Calibration);
}

static void onClearRadar(Navigator* navigator) { RadarDisplay::clear(); }

static void onManualTrigger(Navigator* navigator) {
  state.alarmEnabled = !state.alarmEnabled;
}

static MultiselectOption<RadarMode> modeOptions[] = {
    {RadarMode::Tracking, "TRACKING"},
    {RadarMode::Alert, "ALERT"},
};

class MenuScreen : public Screen {
 private:
  // Widgets
  TextWidget title;
  TextWidget body;
  ButtonWidget stopButton;
  ButtonWidget startButton;
  ButtonWidget recalibrateButton;
  ButtonWidget clearRadarButton;
  MultiselectWidget<RadarMode, 2> radarMode;
  ButtonWidget manualTriggerButton;

 public:
  MenuScreen()
      : Screen(ScreenID::Menu, ScreenID::None),
        title("MENU"),
        body("Manage the system here."),
        stopButton("STOP", onStop),
        startButton("START", onStart),
        recalibrateButton("RECALIBRATE", onRecalibrate),
        clearRadarButton("CLEAR_RADAR", onClearRadar),
        radarMode(ScreenID::Menu, "MODE", modeOptions, &state.mode,
                  "SELECT MODE"),
        manualTriggerButton("TEST_ALARM", onManualTrigger) {
    addWidget(&title);
    addWidget(&stopButton);
    addWidget(&startButton);
    addWidget(&recalibrateButton);
    addWidget(&clearRadarButton);
    addWidget(&radarMode);
    addWidget(&manualTriggerButton);
  }

  void onEnter() override { MatrixDisplay::clear(); }
};