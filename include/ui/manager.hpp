#pragma once

#include <stdint.h>

#include "ui/event.hpp"
#include "ui/navigator.hpp"
#include "ui/screen.hpp"

class UIManager : Navigator {
 private:
  // All registered screens
  Screen* screens[8] = {};

  // Currently active screen
  Screen* current = nullptr;

  // If the screen should redraw at its next update
  bool redrawRequested = true;

  // The time of last render. Used to debounce render by RENDER_INTERVAL_MS.
  uint32_t lastRender = 0;

 public:
  // Initialise the UI Manager.
  void init();

  // Replace the currently active screen with another screen. The previous
  // screen can execute the `Screen::onExit()` event handler, and the new screen
  // will be rendered immediately. This bypasses the render interval.
  void navigate(ScreenID screenId) override;

  // Replace the currently active screen with its parent screen. The previous
  // screen can execute the `Screen::onExit()` event handler, and the new screen
  // will be rendered immediately. This bypasses the render interval.
  void back() override;

  // Registers a new screen to the internal registry. This is used to allow
  // other screens to navigate without creating a new screen or relying on
  // global definitions.
  void registerScreen(Screen* screen) override {
    if (!screen) return;
    uint8_t index = (uint8_t)screen->id;
    if (index >= sizeof(screens) / sizeof(Screen*)) return;
    screens[index] = screen;
  };

  // Deregisters a screen from the internal registry. Useful for making
  // transient screens like nested menus.
  void deregisterScreen(ScreenID screenId) override {
    uint8_t index = (uint8_t)screenId;
    if (index >= sizeof(screens) / sizeof(Screen*)) return;
    if (!screens[index]) return;
    screens[index] = nullptr;
  };

  // Request a redraw at the next update cycle.
  void requestRedraw();

  // If there is a UI event, then the currently active screen, if present, must
  // handle the event. This method then requests a redraw, which will happen
  // after the render interval has elapsed. The render interval will act as a
  // debouncer, absorbing multiple UI events and renders only once. The screen
  // must respond to events immediately without regard to the render interval.
  //
  // The update must be called as fast as possible, and can be called multiple
  // times in one loop if necessary. However, note that the `Screen::update()`
  // is also unconditionally updated every `UIManager::update()`. Be wary of the
  // side effects of updating the screen multiple times in one loop.
  // `UIEvent::None` must be used to indicate no events, which will skip
  // re-rendering the screen if a redraw wasn't explicitly requested.
  void update(UIEvent event);

  // Returns the current screen being rendered.
  Screen* getCurrentScreen() { return current; };
};
