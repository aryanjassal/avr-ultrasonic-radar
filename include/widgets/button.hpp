#pragma once

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
  // Initialise the widget with a label and a callback.
  ButtonWidget(const char* label, void (*callback)(Navigator* navigator));

  // Return the calculated height. It is updated every time the text is updated.
  uint8_t height() override { return 1; };

  // Click events are always passed through even without being selectable.
  void handleEvent(UIEvent event, Navigator* navigator) override;

  // Draw the component.
  void draw(uint8_t yOffset, uint8_t viewportTop) override;
};