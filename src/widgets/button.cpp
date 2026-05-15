#include "widgets/button.hpp"

#include "drivers/display.hpp"

ButtonWidget::ButtonWidget(const char* label,
                           void (*callback)(Navigator* navigator))
    : Widget(nullptr, nullptr), label(label), callback(callback) {}

void ButtonWidget::handleEvent(UIEvent event, Navigator* navigator) {
  if (event == UIEvent::Click && callback != nullptr) { callback(navigator); }
}

void ButtonWidget::draw(uint8_t yOffset, uint8_t viewportTop) {
  int16_t y = (int16_t)yOffset - (int16_t)viewportTop;
  if (y < Display::VIEWPORT_Y_ORIGIN || y >= Display::VIEWPORT_Y_END) {
    return;
  }

  Display::drawText(Display::VIEWPORT_X_ORIGIN, y, label);
  Display::drawChar(Display::VIEWPORT_X_END, y, '>');
}