#pragma once

#include "hardware/lcd.hpp"

#include <stdint.h>

namespace Display {

constexpr uint8_t WIDTH = 16;
constexpr uint8_t HEIGHT = 2;

extern bool dirty;

extern char front[HEIGHT][WIDTH];
extern char back[HEIGHT][WIDTH];

void init(LCD* display);

void clear();
void clearBuffer();

void setCursor(uint8_t x, uint8_t y);
void home();

void write(char character);
void print(const char* str);

void drawChar(uint8_t x, uint8_t y, char c);
void drawText(uint8_t x, uint8_t y, const char* str);
void clearLine(uint8_t y);

void render();
void forceRender();

}  // namespace Display