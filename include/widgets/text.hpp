#pragma once

#include "ui/widget.hpp"

/**
 * Renders text to the screen. Intelligently wraps longer sentences and words as
 * necessary. If a word cannot fit on the current line, then try to fit it into
 * the next line. If the word does not fit into the next line either, then write
 * whatever can fit, and complete the word in the next line. However, if the
 * current position is the beginning of a line and the word can't fit, then
 * start from the current line instead of going to a new line.
 *
 * The height of the widget is then calculated by simulating the flow of the
 * text. This unfortunately cannot be cached to speed up drawing as it would
 * take up too much memory. Every time the text is updated, the height is
 * re-calculated. Normally, the screens will do expensive operations like this
 * during the boot-up phase. However, if the text is updated, the height
 * calculation will be re-run, and can slow down everything. Thus, only widgets
 * with short text are recommended to be updated dynamically.
 */
class TextWidget : public Widget {
 private:
  // The text to render. Can be updated later.
  const char* text;

  // The height is computed once when the text is set to ensure minimal
  // processing during the update loop.
  uint8_t textHeight;

 public:
  // Initialise the widget with some text. This can be changed later.
  TextWidget(const char* text);

  // Update the text. Causes re-calculation of the height data.
  void setText(const char* text);

  // Get the currently stored text.
  const char* getText();

  // Return the calculated height. It is updated every time the text is updated.
  uint8_t height() override { return textHeight; };

  // Draw the component. Text flow canot be cached and happens here. However,
  // the flow is restricted to the viewport to prevent unnecessary rendering, so
  // the performance impact should be minimal.
  void draw(uint8_t yOffset, uint8_t viewportTop) override;
};