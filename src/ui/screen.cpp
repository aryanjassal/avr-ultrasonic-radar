#include "ui/screen.hpp"

#include "drivers/lcd.hpp"
#include "ui/navigator.hpp"

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
  if (total <= LCDDisplay::HEIGHT) return 0;
  return total - LCDDisplay::HEIGHT;
}

Widget* Screen::widgetStartingAt(uint8_t line) {
  uint8_t y = 0;
  for (uint8_t i = 0; i < widgetCount; i++) {
    if (y == line) { return widgets[i]; }
    y += widgets[i]->height();
  }
  return nullptr;
}

Widget* Screen::getNearestWidget(uint8_t line) {
  uint8_t y = 0;
  Widget* last = nullptr;

  for (uint8_t i = 0; i < widgetCount; i++) {
    Widget* widget = widgets[i];
    if (y > line) break;
    last = widget;
    y += widget->height();
  }

  return last;
}

uint8_t Screen::widgetStartLine(Widget* target) {
  uint8_t y = 0;
  for (uint8_t i = 0; i < widgetCount; i++) {
    if (widgets[i] == target) { return y; }
    y += widgets[i]->height();
  }

  return 0;
}

void Screen::handleEvent(UIEvent event) {
  if (widgetCount == 0) return;

  // If a widget is active, then pass all events through.
  if (activeWidget && activeWidget->captureEvents()) {
    activeWidget->handleEvent(event, this->navigator);
    if (!activeWidget->captureEvents()) { activeWidget = nullptr; }
    return;
  }

  // If the event is a click, then attempt passing the event through.
  if (event == UIEvent::Click) {
    Widget* widget = getNearestWidget(cursorLine);

    // If the widget is selectable, then set it as the active widget. Otherwise,
    // only pass the click event through.
    if (widget && widget->selectable()) {
      // Snap viewport and cursor to widget start
      uint8_t start = widgetStartLine(widget);
      cursorLine = start;
      scrollOffset = start;

      // Select widget
      activeWidget = widget;
      activeWidget->onSelect(this->navigator);
      activeWidget->handleEvent(event, this->navigator);
    } else if (widget) {
      widget->handleEvent(event, this->navigator);
    }

    return;
  }

  // Exit early if scrolling is undesired.
  if (scrollDisabled) return;

  // Up and down events correspond to scrolling by default.
  if (event == UIEvent::Up) {
    // Move cursor
    uint8_t total = totalHeight();
    if (cursorLine + 1 >= total) return;
    cursorLine++;
    // Move viewport
    if (cursorLine >= scrollOffset + LCDDisplay::VIEWPORT_HEIGHT) {
      scrollOffset = cursorLine - LCDDisplay::VIEWPORT_HEIGHT + 1;
    }
  } else if (event == UIEvent::Down) {
    // Move cursor
    if (cursorLine == 0) return;
    cursorLine--;
    // Move viewport
    if (cursorLine < scrollOffset) { scrollOffset = cursorLine; }
  }
}

void Screen::draw() {
  uint8_t y = 0;

  for (uint8_t i = 0; i < widgetCount; i++) {
    widgets[i]->draw(y, scrollOffset);
    y += widgets[i]->height();
  }

  // Exit early if cursor shouldn't be rendered, otherwise render cursor.
  if (cursorDisabled) return;
  LCDDisplay::drawChar(0, 0, ' ');
  LCDDisplay::drawChar(0, 1, ' ');
  int16_t cursorY = (int16_t)cursorLine - (int16_t)scrollOffset;
  if (cursorY < LCDDisplay::VIEWPORT_Y_ORIGIN ||
      cursorY >= LCDDisplay::VIEWPORT_Y_END) {
    return;
  }
  LCDDisplay::drawChar(0, cursorY, '>');
}