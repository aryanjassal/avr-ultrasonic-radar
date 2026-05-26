#pragma once

#include "drivers/lcd.hpp"
#include "string.h"
#include "ui/widget.hpp"

/**
 * Draws a single-line button to the screen. Clicking the button executes the
 * specified callback.
 */
class ButtonWidget : public Widget {
 private:
  // The label of the button
  const char* label;

  // Callback function to run on click
  void (*callback)(Navigator* navigator);

 public:
  static void drawButtonLabel(const char* label, uint8_t yOffset,
                              uint8_t viewportTop) {
    int16_t y = (int16_t)yOffset - (int16_t)viewportTop;
    if (y < LCDDisplay::VIEWPORT_Y_ORIGIN || y >= LCDDisplay::VIEWPORT_Y_END) {
      return;
    }

    uint8_t labelLength = strlen(label);
    LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, y, label);
    LCDDisplay::drawChar(LCDDisplay::VIEWPORT_X_ORIGIN + labelLength + 1 >
                                 LCDDisplay::VIEWPORT_X_END - 1
                             ? LCDDisplay::VIEWPORT_X_END - 1
                             : LCDDisplay::VIEWPORT_X_ORIGIN + labelLength + 1,
                         y, '>');
  }

  // Initialise the widget with a label and a callback.
  ButtonWidget(const char* label, void (*callback)(Navigator* navigator))
      : Widget(nullptr, nullptr), label(label), callback(callback) {};

  // Return the calculated height. It is updated every time the text is updated.
  uint8_t height() override { return 1; };

  // Click events are always passed through even without being selectable.
  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event == UIEvent::Click && callback != nullptr) { callback(navigator); }
  };

  // Draw the component.
  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    drawButtonLabel(label, yOffset, viewportTop);
  };
};
