#include "ui/screen.hpp"
#include "drivers/display.hpp"

void Screen::moveCursorUp() {
  // Move cursor
  if (cursorLine == 0) return;
  cursorLine--;
  // Move viewport
  if (cursorLine < scrollOffset) { scrollOffset = cursorLine; }
}

void Screen::moveCursorDown() {
  // Move cursor
  uint8_t total = totalHeight();
  if (cursorLine + 1 >= total) return;
  cursorLine++;
  // Move viewport
  if (cursorLine >= scrollOffset + Display::VIEWPORT_HEIGHT) {
    scrollOffset = cursorLine - Display::VIEWPORT_HEIGHT + 1;
  }
}

void Screen::addWidget(Widget* widget) {
  if (widgetCount >= MAX_WIDGETS) return;
  widgets[widgetCount++] = widget;
}

uint8_t Screen::totalHeight() {
  uint8_t total = 0;
  for (uint8_t i = 0; i < widgetCount; i++) { total += widgets[i]->height(); }
  return total;
}

uint8_t Screen::maxScroll() {
  uint8_t total = totalHeight();
  if (total <= Display::HEIGHT) return 0;
  return total - Display::HEIGHT;
}

Widget* Screen::widgetStartingAt(uint8_t line) {
  uint8_t y = 0;
  for (uint8_t i = 0; i < widgetCount; i++) {
    if (y == line) { return widgets[i]; }
    y += widgets[i]->height();
  }
  return nullptr;
}

void Screen::handleEvent(UIEvent event) {
  if (widgetCount == 0) return;

  // If a widget is active, then pass all events through.
  if (activeWidget && activeWidget->captureEvents()) {
    activeWidget->handleEvent(event);
    if (!activeWidget->captureEvents()) { activeWidget = nullptr; }
    return;
  }

  // If the event is a click, then attempt passing the event through.
  if (event == UIEvent::Click) {
    Widget* widget = widgetStartingAt(scrollOffset);

    if (widget && widget->selectable()) {
      activeWidget = widget;
      activeWidget->handleEvent(event);
    }

    return;
  }

  // Up and down events correspond to scrolling by default.
  if (event == UIEvent::Up) {
    moveCursorDown();
  } else if (event == UIEvent::Down) {
    moveCursorUp();
  }
}

void Screen::draw() {
  uint8_t y = 0;

  for (uint8_t i = 0; i < widgetCount; i++) {
    widgets[i]->draw(y, scrollOffset);
    y += widgets[i]->height();
  }

  // Render cursor
  Display::drawChar(0, 0, ' ');
  Display::drawChar(0, 1, ' ');
  int16_t cursorY = (int16_t)cursorLine - (int16_t)scrollOffset;
  if (cursorY < Display::VIEWPORT_Y_ORIGIN ||
      cursorY >= Display::VIEWPORT_Y_END) {
    return;
  }
  Display::drawChar(0, cursorY, '>');
}