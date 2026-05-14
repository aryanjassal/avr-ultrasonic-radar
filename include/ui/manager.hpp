#pragma once

#include <stdint.h>

#include "ui/event.hpp"
#include "ui/screen.hpp"

class UIManager {
 private:
  // Currently active screen
  Screen* current = nullptr;

  // If the screen should redraw at its next update
  bool redrawRequested = true;

  // The time of last render. Used to debounce render by RENDER_INTERVAL_MS.
  uint32_t lastRender = 0;

 public:
  // Initialise the UI Manager with an initial screen. This can be a welcome or
  // a booting screen while the system prepares itself. Set to `nullptr` to not
  // set an initial screen. If provided, the `Screen::onEnter()` event handler
  // will be executed, immediately followed by `Screen::draw()` and
  // `Display::render()`. This bypasses the render interval.
  void init(Screen* initialScreen);

  // Replace the currently active screen with another screen. The previous
  // screen can execute the `Screen::onExit()` event handler, and the new screen
  // will be rendered immediately. This bypasses the render interval.
  void setScreen(Screen* screen);

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
};