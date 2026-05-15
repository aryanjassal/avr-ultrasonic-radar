#pragma once

// Forward-declare screen to avoid circular dependency.
class Screen;

class Navigator {
  virtual void setScreen(Screen* screen);
};