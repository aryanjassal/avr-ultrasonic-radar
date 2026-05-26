#pragma once

#include "radar/controller.hpp"
#include "radar/display.hpp"
#include "state.hpp"
#include "ui/screen.hpp"
#include "widgets/boolean.hpp"
#include "widgets/button.hpp"
#include "widgets/multiselect.hpp"
#include "widgets/popup.hpp"
#include "widgets/text.hpp"

static MultiselectOption<RadarMode> modeOptions[] = {
    {RadarMode::Tracking, "TRACKING"},
    {RadarMode::Alert, "ALERT"},
};

class MenuScreen : public Screen {
 private:
  // Core widgets
  TextWidget title;
  ButtonWidget stopButton;
  ButtonWidget startButton;
  ButtonWidget recalibrateButton;
  ButtonWidget clearRadarButton;

  // Switch radar alert mode
  MultiselectWidget<RadarMode, 2> radarMode;

  // USART debugging menu
  PopupWidget usartDebuggingMenu;
  BooleanWidget sendAmplitudeInput;
  BooleanWidget sendAngleInput;
  BooleanWidget sendDistanceInput;

  Widget* usartDebuggingWidgets[3] = {&sendAmplitudeInput, &sendAngleInput,
                                      &sendDistanceInput};

  // Callbacks
  static void onStop(Navigator* navigator) { RadarController::stop(); }

  static void onStart(Navigator* navigator) { RadarController::start(); }

  static void onRecalibrate(Navigator* navigator) {
    RadarController::stop();
    state.ready = false;
    navigator->navigate(ScreenID::Calibration);
  }

  static void onClearRadar(Navigator* navigator) { RadarDisplay::clear(); }

  static void onModeChange(Navigator* navigator) {
    state.manualTrackingMode = state.mode == RadarMode::Tracking;
  }

 public:
  MenuScreen()
      : Screen(ScreenID::Menu, ScreenID::None),
        title("MENU"),
        stopButton("STOP", onStop),
        startButton("START", onStart),
        recalibrateButton("RECALIBRATE", onRecalibrate),
        clearRadarButton("CLEAR_RADAR", onClearRadar),
        radarMode(ScreenID::Menu, "MODE", modeOptions, &state.mode,
                  "SELECT MODE", onModeChange),
        usartDebuggingMenu(id, "USART_DEBUG", "USART DEBUG",
                           usartDebuggingWidgets, 3),
        sendAmplitudeInput("SEND_AMPLITUDE", &state.sendAmplitude),
        sendAngleInput("SEND_ANGLE", &state.sendAngle),
        sendDistanceInput("SEND_DISTANCE", &state.sendDistance) {
    addWidget(&title);
    addWidget(&stopButton);
    addWidget(&startButton);
    addWidget(&recalibrateButton);
    addWidget(&clearRadarButton);
    addWidget(&radarMode);
    addWidget(&usartDebuggingMenu);
  }

  void onEnter() override {
    if (!state.manualTrackingMode) { state.mode = RadarMode::Alert; }
  }
};
