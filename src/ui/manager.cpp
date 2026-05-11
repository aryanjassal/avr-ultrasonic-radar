#include "ui/manager.hpp"
#include "drivers/display.hpp"

void UIManager::init(Screen* initialScreen) {
  current = initialScreen;
  redrawRequested = true;

  if (current) {
    current->onEnter();
    current->draw();
    Display::render();
  }
}

void UIManager::setScreen(Screen* screen) {
  if (current) { current->onExit(); }

  current = screen;
  redrawRequested = true;

  Display::clearBuffer();

  if (current) {
    current->onEnter();
    current->draw();
  }

  Display::render();
}

void UIManager::requestRedraw() { redrawRequested = true; }

void UIManager::update(uint32_t now, UIEvent event) {
  if (!current) return;

  if (event != UIEvent::None) {
    current->handleEvent(event);
    redrawRequested = true;
  }

  current->update(now);

  if (redrawRequested && ((now - lastRender) >= RENDER_INTERVAL_MS)) {
    Display::clearBuffer();
    current->draw();
    Display::render();

    redrawRequested = false;
    lastRender = now;
  }
}