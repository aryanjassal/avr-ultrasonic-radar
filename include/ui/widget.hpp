#pragma once

#include <stdint.h>

#include "ui/event.hpp"
#include "ui/navigator.hpp"

class Widget {
 protected:
  void (*onSelectCallback)(Navigator* navigator);
  void (*onDeselectCallback)(Navigator* navigator);

 public:
  Widget(void (*onSelectCallback)(Navigator* navigator),
         void (*onDeselectCallback)(Navigator* navigator))
      : onSelectCallback(onSelectCallback),
        onDeselectCallback(onDeselectCallback) {};

  virtual ~Widget() {}

  // Virtual height in lines.
  virtual uint8_t height() = 0;

  // Is the widget selectable by the cursor?
  virtual bool selectable() { return false; }

  // If the widget is currently capturing events.
  virtual bool captureEvents() { return false; }

  // Event handling endpoint if the widget is currently handling events. If the
  // widget never handles events, this can be left unimplemented.
  virtual void handleEvent(UIEvent event, Navigator* navigator) {}

  // The `Widget::onSelect()` must be executed by the parent screen when the
  // widget obtains focus. This allows for special states with information of
  // the currently active widget, allowing for dynamic previews.
  void onSelect(Navigator* navigator) {
    if (onSelectCallback != nullptr) { onSelectCallback(navigator); }
  }

  // The `Widget::onDeselect()` must be executed by the parent screen when the
  // widget loses focus. This allows for special states with information of
  // the currently active widget, allowing for dynamic previews.
  void onDeselect(Navigator* navigator) {
    if (onDeselectCallback != nullptr) { onDeselectCallback(navigator); }
  }

  // Draw into virtual layout space. The viewport is the relative space for this
  // widget where it will be placed with no knowledge of any surrounding
  // context.
  virtual void draw(uint8_t yOffset, uint8_t viewportTop) = 0;
};
