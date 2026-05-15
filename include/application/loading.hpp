#pragma once

#include "ui/screen.hpp"
#include "widgets/text.hpp"

class LoadingScreen : public Screen {
 private:
  // Widgets
  TextWidget text;

 public:
  LoadingScreen() : text("SONIC RADAR\nLoading...") {
    disableCursor(true);
    disableScroll(true);
    addWidget(&text);
  }
};