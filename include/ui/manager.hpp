#pragma once

#include <stdint.h>

#include "ui/event.hpp"
#include "ui/screen.hpp"

class UIManager {
 public:
  void init(Screen* initialScreen);

  void setScreen(Screen* screen);

  void update(uint32_t now, UIEvent event);

  void requestRedraw();

 private:
  Screen* current = nullptr;
  bool redrawRequested = true;
  uint32_t lastRender = 0;

  static constexpr uint32_t RENDER_INTERVAL_MS = 100;
};