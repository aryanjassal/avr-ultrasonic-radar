#pragma once

#include "drivers/pin.hpp"

class Button {
  InputPin& pin;
  bool lastState = true;   // Last raw sample
  bool lastStable = true;  // Last debounced state
  uint32_t lastChange = 0;

 public:
  Button(InputPin& p);

  bool pressed();
};