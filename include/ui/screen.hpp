#pragma once

#include "ui/event.hpp"

#include <stdint.h>

class Screen {
 public:
  virtual ~Screen() {}

  virtual void onEnter() {}

  virtual void onExit() {}

  virtual void handleEvent(UIEvent event) = 0;

  virtual void update(uint32_t now) {}

  virtual void draw() = 0;
};