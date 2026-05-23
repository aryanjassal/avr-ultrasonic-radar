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

template <uint8_t COUNT>
class PopupScreen : public Screen {
 private:
  TextWidget title;
  ButtonWidget backButton;
  bool configured = false;

 public:
  PopupScreen(ScreenID parent, const char* titleText, Widget* /*children*/[COUNT])
      : Screen(ScreenID::Popup, parent),
        title(titleText),
        backButton("BACK", [](Navigator* navigator) { navigator->back(); }) {}

  void configure(Widget* children[COUNT]) {
    if (configured) return;

    addWidget(&title);

    for (uint8_t i = 0; i < COUNT; i++) {
      if (children[i]) { addWidget(children[i]); }
    }

    addWidget(&backButton);
    configured = true;
  }
};

template <uint8_t COUNT>
class PopupWidget : public Widget {
 private:
  ButtonWidget launcher;
  PopupScreen<COUNT> popup;
  Widget** widgets;

 public:
  PopupWidget(ScreenID parent, const char* label, const char* popupTitle,
              Widget* widgets[COUNT])
      : Widget(nullptr, nullptr),
        launcher(label, nullptr),
        popup(parent, popupTitle, widgets),
        widgets(widgets) {}

  uint8_t height() override { return launcher.height(); }

  void draw(uint8_t y, uint8_t top) override { launcher.draw(y, top); }

  void handleEvent(UIEvent event, Navigator* navigator) override {
    if (event != UIEvent::Click) return;
    popup.id = ScreenID::Popup;
    popup.configure(widgets);
    navigator->registerScreen(&popup);
    navigator->navigate(ScreenID::Popup);
  }
};
