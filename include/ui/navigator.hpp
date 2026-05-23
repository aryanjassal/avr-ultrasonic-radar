#pragma once

#include "application/screens.hpp"

// Forward-declare screen to avoid circular dependency.
class Screen;

class Navigator {
 public:
  virtual void navigate(ScreenID id) = 0;
  virtual void back() = 0;
  virtual void registerScreen(Screen* screen) = 0;
  virtual void deregisterScreen(ScreenID id) = 0;
};