#pragma once

#include "drivers/lcd.hpp"
#include "ui/navigator.hpp"
#include "ui/widget.hpp"

/**
 * The boolean widget does not need to capture focus, as clicks are always
 * passed through, and simple click can toggle the state without requiring event
 * capture.
 */
class BooleanWidget : public Widget {
 public:
  // Note: the label must be smaller than the available width of the viewport.
  // Any excess data will be trimmed automatically.
  BooleanWidget(const char* label, bool* value)
      : Widget(nullptr, nullptr), label(label), value(value) {}

  uint8_t height() override { return 2; }

  // Clicks are always forwarded. If a click is detected, then toggle the state.
  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event == UIEvent::Click) { *value = !(*value); }
    return;
  }

  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    int16_t screenY = (int16_t)yOffset - (int16_t)viewportTop;
    if (screenY <= -2 || screenY >= LCDDisplay::VIEWPORT_HEIGHT) { return; }

    // LABEL
    if (screenY >= 0 && screenY < LCDDisplay::VIEWPORT_HEIGHT) {
      LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, screenY, label);
    }

    // VALUE
    if (screenY + 1 >= 0 && screenY + 1 < LCDDisplay::VIEWPORT_HEIGHT) {
      if (*value) {
        LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, screenY + 1,
                             "[TRUE]");
      } else {
        LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, screenY + 1,
                             "[FALSE]");
      }
    }
  }

 private:
  const char* label;
  bool* value;
};
