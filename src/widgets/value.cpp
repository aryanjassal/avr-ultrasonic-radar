// #include "widgets/value.hpp"

// #include <stdlib.h>

// #include "drivers/display.hpp"

// ValueWidget::ValueWidget(const char* label, int16_t* value, int16_t minValue,
//                          int16_t maxValue)
//     : label(label),
//       value(value),
//       minValue(minValue),
//       maxValue(maxValue),
//       capturing(false) {}

// uint8_t ValueWidget::height() { return 2; }

// bool ValueWidget::focusable() { return true; }

// void ValueWidget::handleEvent(UIEvent event) {
//   if (event == UIEvent::Click) {
//     capturing = !capturing;
//     return;
//   }

//   if (!capturing) return;

//   if (event == UIEvent::Up) {
//     if (*value < maxValue) (*value)++;
//   }

//   if (event == UIEvent::Down) {
//     if (*value > minValue) (*value)--;
//   }
// }

// void ValueWidget::draw(uint8_t yOffset, uint8_t viewportTop, bool focused) {
//   int16_t screenY = (int16_t)yOffset - (int16_t)viewportTop;

//   if (screenY <= -2 || screenY >= Display::HEIGHT) { return; }

//   // LABEL LINE

//   if (screenY >= 0 && screenY < Display::HEIGHT) {
//     if (focused)
//       Display::drawChar(0, screenY, '>');
//     else
//       Display::drawChar(0, screenY, ' ');

//     Display::drawText(2, screenY, label);
//   }

//   // VALUE LINE

//   if (screenY + 1 >= 0 && screenY + 1 < Display::HEIGHT) {
//     if (focused)
//       Display::drawChar(0, screenY + 1, '>');
//     else
//       Display::drawChar(0, screenY + 1, ' ');

//     char buffer[16];

//     itoa(*value, buffer, 10);

//     if (capturing) {
//       Display::drawChar(2, screenY + 1, '[');

//       Display::drawText(3, screenY + 1, buffer);

//       uint8_t len = 0;

//       while (buffer[len]) len++;

//       Display::drawChar(3 + len, screenY + 1, ']');
//     } else {
//       Display::drawText(3, screenY + 1, buffer);
//     }
//   }
// }

// bool ValueWidget::captureEvents() { return capturing; }