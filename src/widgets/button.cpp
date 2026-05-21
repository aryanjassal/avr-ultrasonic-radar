#include "widgets/button.hpp"

#include "drivers/lcd.hpp"

ButtonWidget::ButtonWidget(const char* label,
                           void (*callback)(Navigator* navigator))
    : Widget(nullptr, nullptr), label(label), callback(callback) {}

void ButtonWidget::handleEvent(UIEvent event, Navigator* navigator) {
  if (event == UIEvent::Click && callback != nullptr) { callback(navigator); }
}

void ButtonWidget::draw(uint8_t yOffset, uint8_t viewportTop) {
  int16_t y = (int16_t)yOffset - (int16_t)viewportTop;
  if (y < LCDDisplay::VIEWPORT_Y_ORIGIN || y >= LCDDisplay::VIEWPORT_Y_END) {
    return;
  }

  LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, y, label);
  LCDDisplay::drawChar(LCDDisplay::VIEWPORT_X_END, y, '>');
}