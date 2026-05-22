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
  TextWidget body;
  ButtonWidget backButton;

 public:
  PopupScreen(ScreenID parent, const char* titleText, const char* bodyText)
      : Screen(ScreenID::Popup, parent),
        title(titleText),
        body(bodyText),
        backButton("BACK", [](Navigator* navigator) { navigator->back(); }) {
    addWidget(&title);
    addWidget(&body);
    addWidget(&backButton);
  }
};

class PopupWidget : public Widget {
 private:
  ButtonWidget launcher;
  PopupScreen popup;

 public:
  PopupWidget(ScreenID parent, const char* label, const char* popupTitle,
              const char* popupBody)
      : Widget(nullptr, nullptr),
        launcher(label, nullptr),
        popup(parent, popupTitle, popupBody) {}

  uint8_t height() override { return launcher.height(); }

  void draw(uint8_t y, uint8_t top) override { launcher.draw(y, top); }

  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event != UIEvent::Click) return;
    navigator->registerScreen(ScreenID::Popup, &popup);
    navigator->navigate(ScreenID::Popup);
  }
};