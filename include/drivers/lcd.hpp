#pragma once

#include "hardware/lcd.hpp"

#include <stdint.h>

namespace LCDDisplay {

// Display resolution to build a framebuffer
constexpr uint8_t WIDTH = 16;
constexpr uint8_t HEIGHT = 2;

// To account for the cursor gutter on the left. One character space must be
// left to allow it.
constexpr uint8_t VIEWPORT_WIDTH = 15;
constexpr uint8_t VIEWPORT_X_ORIGIN = 1;
constexpr uint8_t VIEWPORT_HEIGHT = 2;
constexpr uint8_t VIEWPORT_Y_ORIGIN = 0;
constexpr uint8_t VIEWPORT_X_END = VIEWPORT_X_ORIGIN + VIEWPORT_WIDTH;
constexpr uint8_t VIEWPORT_Y_END = VIEWPORT_Y_ORIGIN + VIEWPORT_HEIGHT;

// Was the framebuffer modified?
extern bool dirty;

// Initialise the display with the given LCD. Note that the LCD should be
// initialised already.
void init(LCD* display);

// Clears the back buffer and resets the cursor position. Note that only the
// buffer is cleared and the dirty flag set. As such, the buffer can be written
// to, or rendered.
//
// Refer to `LCDDisplay::clear()` to clear the LCD screen directly.
void clearBuffer();

// Writes a character at the specified x and y coordinate to the framebuffer.
// The display must be rendered to update the LCD panel.
void drawChar(uint8_t x, uint8_t y, char c);

// Prints a string originating at the specified x and y coordinate to the
// framebuffer. Will cut off the string if the length exceeds the width of the
// panel Will cut off the string if the length exceeds the width of the panel.
// The display must be rendered to update the LCD panel.
void drawText(uint8_t x, uint8_t y, const char* str);

// Replaces the specified line with spaces to clear it.
void clearLine(uint8_t y);

// The front buffer is currently being displayed, and the back buffer is
// currently being rendered to. When rendering, each position in the framebuffer
// is diffed to find the updated section, and the changed section is updated
// while skipping the unchanged section. The back buffer will overwrite the old
// data in the front buffer until both buffers are synchronised, at which point
// the back buffer can be updated with the new content.
//
// This method mutates the LCD state, in contrast to other methods only
// modifying the back buffer for the next render.
void render(bool force = false);

// Clears both the front and back buffer while also clearing the LCD screen.
//
// This method mutates the LCD state, in contrast to other methods only
// modifying the back buffer for the next render.
void clear();

}  // namespace LCDDisplay