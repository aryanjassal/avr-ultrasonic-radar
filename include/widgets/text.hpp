#pragma once

#include "drivers/lcd.hpp"
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

  // Calculate the word length. A word is anything split by spaces or newlines.
  static uint8_t wordLength(const char* str) {
    uint8_t len = 0;
    while (str[len] && str[len] != ' ' && str[len] != '\n') { len++; }
    return len;
  }

  static uint8_t calculateHeight(const char* text) {
    // Set up the variables used for the calculation
    uint8_t lines = 1;
    uint8_t x = LCDDisplay::VIEWPORT_X_ORIGIN;
    const char* ptr = text;

    while (*ptr) {
      // Newlines unconditionally increase total lines by 1
      if (*ptr == '\n') {
        lines++;
        x = LCDDisplay::VIEWPORT_X_ORIGIN;
        ptr++;
        continue;
      }

      // Move word to next line if necessary
      uint8_t len = wordLength(ptr);
      if (x != LCDDisplay::VIEWPORT_X_ORIGIN &&
          (x + len) > LCDDisplay::VIEWPORT_WIDTH) {
        lines++;
        x = LCDDisplay::VIEWPORT_X_ORIGIN;
      }

      // Calculate if the word takes multiple lines or not
      while (*ptr && *ptr != ' ' && *ptr != '\n') {
        // Long-word wrapping
        if (x >= LCDDisplay::VIEWPORT_X_END) {
          lines++;
          x = LCDDisplay::VIEWPORT_X_ORIGIN;
        }

        // Happy path
        x++;
        ptr++;
      }

      // If word break, then continue
      if (*ptr == ' ') {
        if (x < LCDDisplay::VIEWPORT_X_END) x++;
        ptr++;
      }
    }

    return lines;
  }

 public:
  TextWidget()
      : Widget(nullptr, nullptr),
        text(""),
        textHeight(1) {};

  // Initialise the widget with some text. This can be changed later.
  TextWidget(const char* text)
      : Widget(nullptr, nullptr),
        text(text),
        textHeight(calculateHeight(text)) {};

  // Update the text. Causes re-calculation of the height data.
  void setText(const char* text) {
    this->text = text;
    this->textHeight = calculateHeight(text);
  };

  // Get the currently stored text.
  const char* getText() { return text; };

  // Return the calculated height. It is updated every time the text is updated.
  uint8_t height() override { return textHeight; };

  // Draw the component. Text flow canot be cached and happens here. However,
  // the flow is restricted to the viewport to prevent unnecessary rendering, so
  // the performance impact should be minimal.
  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    // Set up relative positioning in the viewport
    int16_t cy = (int16_t)yOffset - (int16_t)viewportTop;
    uint8_t cx = LCDDisplay::VIEWPORT_X_ORIGIN;
    const char* ptr = text;

    // Only render what is currently visible in the viewport
    while (*ptr && cy < LCDDisplay::VIEWPORT_Y_END) {
      // Newlines unconditionally require a new line
      if (*ptr == '\n') {
        cx = LCDDisplay::VIEWPORT_X_ORIGIN;
        cy++;
        ptr++;
        continue;
      }

      // Wrap word if necessary
      uint8_t len = wordLength(ptr);
      if (cx != LCDDisplay::VIEWPORT_X_ORIGIN &&
          (cx + len) > LCDDisplay::VIEWPORT_X_END) {
        cx = LCDDisplay::VIEWPORT_X_ORIGIN;
        cy++;
      }

      // Flow text
      while (*ptr && *ptr != ' ' && *ptr != '\n') {
        // Long word wrapping
        if (cx >= LCDDisplay::VIEWPORT_X_END) {
          cx = LCDDisplay::VIEWPORT_X_ORIGIN;
          cy++;
        }

        // Visible viewport
        if (cy >= LCDDisplay::VIEWPORT_Y_ORIGIN &&
            cy < LCDDisplay::VIEWPORT_Y_END) {
          LCDDisplay::drawChar(cx, cy, *ptr);
        }
        cx++;
        ptr++;

        // Abort if below viewport
        if (cy >= LCDDisplay::VIEWPORT_Y_END) return;
      }

      // Break word and render said word break
      if (*ptr == ' ') {
        if (cx >= LCDDisplay::VIEWPORT_X_END) {
          cx = LCDDisplay::VIEWPORT_X_ORIGIN;
          cy++;
        }
        if (cy >= LCDDisplay::VIEWPORT_Y_ORIGIN &&
            cy < LCDDisplay::VIEWPORT_Y_END) {
          LCDDisplay::drawChar(cx, cy, ' ');
        }
        cx++;
        ptr++;
      }
    }
  };
};
