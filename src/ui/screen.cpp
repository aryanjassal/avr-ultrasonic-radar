#include "ui/screen.hpp"
#include "drivers/display.hpp"

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
    if (scrollOffset < maxScroll()) { scrollOffset++; }
  } else if (event == UIEvent::Down) {
    if (scrollOffset > 0) { scrollOffset--; }
  }
}

void Screen::draw() {
  uint8_t y = 0;

  for (uint8_t i = 0; i < widgetCount; i++) {
    widgets[i]->draw(y, scrollOffset);
    y += widgets[i]->height();
  }
}