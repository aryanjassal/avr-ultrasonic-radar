#include "drivers/display.hpp"

#include <stdint.h>

namespace Display {

// Was the framebuffer modified?
bool dirty = false;

// Using double-buffering to render data
char front[HEIGHT][WIDTH];
char back[HEIGHT][WIDTH];

// The active LCD panel being used as the display
static LCD* lcd = nullptr;

// Cursor position within the screen bounds
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Fill the whole buffer with the specified character
static void fill(char buffer[HEIGHT][WIDTH], char c) {
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) { buffer[y][x] = c; }
  }
}

void init(LCD* display) {
  // Set the active panel for the display
  lcd = display;

  // Initialise framebuffers and cursor positions
  fill(front, '\0');
  fill(back, ' ');
  cursor_x = 0;
  cursor_y = 0;

  // Force-render the framebuffer
  render(true);
}

void clearBuffer() {
  fill(back, ' ');
  cursor_x = 0;
  cursor_y = 0;
  dirty = true;
}

void clear() {
  clearBuffer();
  if (lcd) lcd->clear();
  fill(front, ' ');
  dirty = false;
}

void drawChar(uint8_t x, uint8_t y, char c) {
  if (x >= WIDTH || y >= HEIGHT) return;

  if (back[y][x] != c) {
    back[y][x] = c;
    dirty = true;
  }
}

void drawText(uint8_t x, uint8_t y, const char* str) {
  if (y >= HEIGHT) return;

  while (*str && x < WIDTH) {
    drawChar(x, y, *str);
    x++;
    str++;
  }
}

void clearLine(uint8_t y) {
  if (y >= HEIGHT) return;
  for (uint8_t x = 0; x < WIDTH; x++) { drawChar(x, y, ' '); }
}

void render(bool force) {
  if (!lcd || (!force && !dirty)) return;

  // Diff each position and update only if contents changed
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      if (force || front[y][x] != back[y][x]) {
        lcd->setCursor(x, y);
        lcd->print(back[y][x]);
        front[y][x] = back[y][x];
      }
    }
  }

  dirty = false;
}

}  // namespace Display