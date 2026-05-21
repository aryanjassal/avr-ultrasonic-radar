#include "ui/manager.hpp"
#include "drivers/lcd.hpp"
#include "drivers/timer.hpp"
#include "hardware/dial.hpp"

// The display will be rendered every 100ms at most to minimise rendering
// load.
static constexpr uint32_t RENDER_INTERVAL_MS = 100;

void UIManager::init(Screen* initialScreen) {
  Dial::reset();
  current = initialScreen;
  redrawRequested = true;

  if (current) {
    current->onEnter();
    current->draw();
    LCDDisplay::render();
  }
}

void UIManager::setScreen(Screen* screen) {
  if (current) { current->onExit(); }

  current = screen;
  redrawRequested = true;
  LCDDisplay::clearBuffer();
  Dial::reset();

  if (current) {
    current->onEnter();
    current->draw();
  }

  LCDDisplay::render();
}

void UIManager::requestRedraw() { redrawRequested = true; }

void UIManager::update(UIEvent event) {
  if (!current) return;
  uint32_t now = millis();

  // Only handle event if an actual event was provided
  if (event != UIEvent::None) {
    current->handleEvent(event);
    redrawRequested = true;
  }

  // Every update also triggers an update on the current screen
  current->update(now);

  // Render to the screen
  if (redrawRequested && ((now - lastRender) >= RENDER_INTERVAL_MS)) {
    LCDDisplay::clearBuffer();
    current->draw();
    LCDDisplay::render();

    redrawRequested = false;
    lastRender = now;
  }
}