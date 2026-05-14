#pragma once

#include <stdint.h>

#include "event.hpp"

class Widget {
 public:
  virtual ~Widget() {}

  // Virtual height in lines.
  virtual uint8_t height() = 0;

  // Is the widget selectable by the cursor?
  virtual bool selectable() { return false; }

  // If the widget is currently capturing events.
  virtual bool captureEvents() { return false; }

  // Event handling endpoint if the widget is currently handling events. If the
  // widget never handles events, this can be left unimplemented.
  virtual void handleEvent(UIEvent event) {}

  // Draw into virtual layout space. The viewport is the relative space for this
  // widget where it will be placed with no knowledge of any surrounding
  // context.
  virtual void draw(uint8_t yOffset, uint8_t viewportTop) = 0;
};