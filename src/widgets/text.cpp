#include "widgets/text.hpp"

#include "drivers/lcd.hpp"

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

TextWidget::TextWidget(const char* text)
    : Widget(nullptr, nullptr), text(text), textHeight(calculateHeight(text)) {}

void TextWidget::setText(const char* text) {
  this->text = text;
  this->textHeight = calculateHeight(text);
}

const char* TextWidget::getText() { return text; }

void TextWidget::draw(uint8_t yOffset, uint8_t viewportTop) {
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
}