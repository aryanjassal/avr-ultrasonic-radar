#pragma once

#include "radar/controller.hpp"
#include "state.hpp"
#include "ui/screen.hpp"
#include "widgets/button.hpp"
#include "widgets/text.hpp"

class AlarmScreen : public Screen {
 private:
  // Widgets
  TextWidget title;
  TextWidget body;
  ButtonWidget muteButton;
  ButtonWidget startButton;
  ButtonWidget backButton;

  // Callbacks
  static void onMute(Navigator* navigator) { state.alarmEnabled = false; }

  static void onStart(Navigator* navigator) { RadarController::start(); }

  static void onBack(Navigator* navigator) { navigator->back(); }

 public:
  AlarmScreen()
      : Screen(ScreenID::Alarm, ScreenID::None),
        title("ALARM"),
        body("Intruder alert"),
        muteButton("MUTE", onMute),
        startButton("START_SCAN", onStart),
        backButton("BACK", onBack) {
    addWidget(&title);
    addWidget(&body);
    addWidget(&muteButton);
    addWidget(&startButton);
    addWidget(&backButton);
  }

  void onExit() override {
    state.alarmEnabled = false;
    RadarController::start();
  }
};