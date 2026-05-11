#include "drivers/display.hpp"

#include <stdint.h>

namespace Display {

bool dirty = false;

char front[HEIGHT][WIDTH];
char back[HEIGHT][WIDTH];

static LCD* lcd = nullptr;

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void fill(char buffer[HEIGHT][WIDTH], char c) {
  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) { buffer[y][x] = c; }
  }
}

void init(LCD* display) {
  lcd = display;

  fill(front, '\0');
  fill(back, ' ');

  cursor_x = 0;
  cursor_y = 0;

  dirty = true;
  forceRender();
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

void home() {
  cursor_x = 0;
  cursor_y = 0;

  if (lcd) lcd->home();
}

void setCursor(uint8_t x, uint8_t y) {
  if (x >= WIDTH) x = WIDTH - 1;
  if (y >= HEIGHT) y = HEIGHT - 1;

  cursor_x = x;
  cursor_y = y;
}

void drawChar(uint8_t x, uint8_t y, char c) {
  if (x >= WIDTH || y >= HEIGHT) return;

  if (back[y][x] != c) {
    back[y][x] = c;
    dirty = true;
  }
}

void write(char character) {
  if (character == '\n') {
    cursor_x = 0;
    if (cursor_y + 1 < HEIGHT) cursor_y++;
    return;
  }

  if (cursor_x >= WIDTH || cursor_y >= HEIGHT) return;
  drawChar(cursor_x, cursor_y, character);
  cursor_x++;

  if (cursor_x >= WIDTH) {
    cursor_x = 0;
    if (cursor_y + 1 < HEIGHT) cursor_y++;
  }
}

void print(const char* str) {
  while (*str) { write(*str++); }
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

void render() {
  if (!lcd || !dirty) return;

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      if (front[y][x] != back[y][x]) {
        lcd->setCursor(x, y);
        lcd->print(back[y][x]);
        front[y][x] = back[y][x];
      }
    }
  }

  dirty = false;
}

void forceRender() {
  if (!lcd) return;

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) { front[y][x] = '\0'; }
  }

  dirty = true;
  render();
}

}  // namespace Display