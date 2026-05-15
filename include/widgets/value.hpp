#pragma once

#include <stdlib.h>

#include "drivers/display.hpp"
#include "ui/navigator.hpp"
#include "ui/widget.hpp"

/**
 * The generics here allow for the usage of any type for the values, as long as
 * they are incrementable and decrementable. This is designed for different
 * types of integers.
 */
template <typename T>
class ValueWidget : public Widget {
 public:
  ValueWidget(const char* label, T* value, T minValue, T maxValue,
              void (*onSelectCallback)(Navigator* navigator) = nullptr,
              void (*onDeselectCallback)(Navigator* navigator) = nullptr)
      : Widget(onSelectCallback, onDeselectCallback),
        label(label),
        value(value),
        minValue(minValue),
        maxValue(maxValue),
        capturing(false) {}

  uint8_t height() override { return 2; }

  bool selectable() override { return true; }

  bool captureEvents() override { return capturing; }

  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event == UIEvent::Click) {
      capturing = !capturing;
      if (!capturing) { onDeselect(navigator); }
      return;
    }

    if (!capturing) return;
    if (event == UIEvent::Up && *value < maxValue) { (*value)++; }
    if (event == UIEvent::Down && *value > minValue) { (*value)--; }
  }

  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    int16_t screenY = (int16_t)yOffset - (int16_t)viewportTop;

    if (screenY <= -2 || screenY >= Display::VIEWPORT_HEIGHT) { return; }

    // LABEL
    if (screenY >= 0 && screenY < Display::VIEWPORT_HEIGHT) {
      Display::drawText(Display::VIEWPORT_X_ORIGIN, screenY, label);
    }

    // VALUE
    if (screenY + 1 >= 0 && screenY + 1 < Display::VIEWPORT_HEIGHT) {
      char buffer[Display::VIEWPORT_WIDTH];
      ltoa(*value, buffer, 10);
      if (capturing) {
        Display::drawChar(Display::VIEWPORT_X_ORIGIN, screenY + 1, '[');
        Display::drawText(Display::VIEWPORT_X_ORIGIN + 1, screenY + 1, buffer);
        uint8_t len = 0;
        while (buffer[len]) len++;
        Display::drawChar(Display::VIEWPORT_X_ORIGIN + 1 + len, screenY + 1,
                          ']');
      } else {
        Display::drawText(Display::VIEWPORT_X_ORIGIN + 1, screenY + 1, buffer);
      }
    }
  }

 private:
  const char* label;
  T* value;
  T minValue;
  T maxValue;
  bool capturing;
};
