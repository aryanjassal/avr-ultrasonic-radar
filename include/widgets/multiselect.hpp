#pragma once

#include "application/screens.hpp"
#include "drivers/lcd.hpp"
#include "ui/screen.hpp"
#include "ui/widget.hpp"
#include "widgets/text.hpp"

template <typename T>
struct MultiselectOption {
  T value;
  const char* label;
};

template <typename T, uint8_t COUNT>
class MultiselectScreen : public Screen {
 private:
  TextWidget title;
  TextWidget* entries[COUNT];
  MultiselectOption<T>* options;
  T* boundValue;

 public:
  MultiselectScreen(ScreenID parent, const char* screenTitle,
                    MultiselectOption<T>* options, T* boundValue)
      : Screen(ScreenID::Popup, parent),
        title(screenTitle),
        options(options),
        boundValue(boundValue) {
    addWidget(&title);

    for (uint8_t i = 0; i < COUNT; i++) {
      entries[i] = new TextWidget(options[i].label);
      addWidget(entries[i]);
    }
  }

  void handleEvent(UIEvent event) override {
    if (event != UIEvent::Click) {
      Screen::handleEvent(event);
      return;
    }

    Widget* widget = getNearestWidget(cursorLine);

    for (uint8_t i = 0; i < COUNT; i++) {
      if (widget == entries[i]) {
        *boundValue = options[i].value;
        navigator->navigate(parent);
        navigator->deregisterScreen(ScreenID::Popup);
        return;
      }
    }
  }
};

template <typename T, uint8_t COUNT>
class MultiselectWidget : public Widget {
 private:
  const char* label;
  MultiselectOption<T>* options;
  T* boundValue;
  MultiselectScreen<T, COUNT> popup;

 public:
  MultiselectWidget(ScreenID parent, const char* label,
                    MultiselectOption<T>* options, T* boundValue,
                    const char* popupTitle)
      : Widget(nullptr, nullptr),
        label(label),
        options(options),
        boundValue(boundValue),
        popup(parent, popupTitle, options, boundValue) {}

  uint8_t height() override { return 2; }

  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event != UIEvent::Click) return;
    navigator->registerScreen(ScreenID::Popup, &popup);
    navigator->navigate(ScreenID::Popup);
  }

  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    // Draw label + current option
    int16_t screenY = (int16_t)yOffset - (int16_t)viewportTop;
    if (screenY <= -2 || screenY >= LCDDisplay::VIEWPORT_HEIGHT) { return; }

    // LABEL
    if (screenY >= 0 && screenY < LCDDisplay::VIEWPORT_HEIGHT) {
      LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN, screenY, label);
    }

    // VALUE
    if (screenY + 1 >= 0 && screenY + 1 < LCDDisplay::VIEWPORT_HEIGHT) {
      const char* bound = nullptr;
      for (int i = 0; i < COUNT; i++) {
        if (options[i].value == *boundValue) {
          bound = options[i].label;
          break;
        }
      }
      LCDDisplay::drawText(LCDDisplay::VIEWPORT_X_ORIGIN + 1, screenY + 1,
                           bound);
    }
  }
};