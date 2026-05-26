#pragma once

#include "application/screens.hpp"
#include "ui/screen.hpp"
#include "ui/widget.hpp"
#include "widgets/button.hpp"
#include "widgets/text.hpp"

struct PopupAction {
  const char* label;
  void (*callback)(void);
};

class PopupScreen : public Screen {
 private:
  TextWidget title;
  ButtonWidget backButton;

 public:
  PopupScreen()
      : Screen(ScreenID::Popup, ScreenID::None),
        title(""),
        backButton("BACK", [](Navigator* navigator) { navigator->back(); }) {}

  void configure(ScreenID parent, const char* titleText, Widget** children,
                 uint8_t childCount) {
    this->parent = parent;
    title.setText(titleText);
    widgetCount = 0;

    addWidget(&title);
    for (uint8_t i = 0; i < childCount; i++) {
      if (children[i]) { addWidget(children[i]); }
    }
    addWidget(&backButton);
  }
};

extern PopupScreen popup;

class PopupWidget : public Widget {
 private:
  ScreenID parent;
  const char* label;
  const char* title;
  Widget** widgets;
  uint8_t widgetCount;

 public:
  PopupWidget(ScreenID parent, const char* label, const char* popupTitle,
              Widget** widgets, uint8_t widgetCount)
      : Widget(nullptr, nullptr),
        parent(parent),
        label(label),
        title(popupTitle),
        widgets(widgets),
        widgetCount(widgetCount) {}

  uint8_t height() override { return 1; }

  void draw(uint8_t yOffset, uint8_t viewportTop) override {
    ButtonWidget::drawButtonLabel(label, yOffset, viewportTop);
  }

  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event != UIEvent::Click) return;
    popup.configure(parent, title, widgets, widgetCount);
    navigator->registerScreen(&popup);
    navigator->navigate(ScreenID::Popup);
  }
};
