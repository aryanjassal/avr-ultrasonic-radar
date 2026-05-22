#pragma once

#include "application/screens.hpp"
#include "ui/screen.hpp"
#include "widgets/text.hpp"

class LoadingScreen : public Screen {
 private:
  // Widgets
  TextWidget text;

 public:
  LoadingScreen()
      : Screen(ScreenID::Loading, ScreenID::None),
        text("SONIC RADAR\nLoading...") {
    disableCursor(true);
    disableScroll(true);
    addWidget(&text);
  }
};