#pragma once

#include <stdint.h>

#include "application/screens.hpp"
#include "ui/event.hpp"
#include "ui/widget.hpp"

// Forward-declare navigator to avoid circular dependency.
class Navigator;

class Screen {
 public:
  static constexpr uint8_t MAX_WIDGETS = 10;

  Screen(ScreenID self, ScreenID parent) : id(self), parent(parent) {}

  virtual ~Screen() = default;

  // Executed when the UI manager is entering this screen.
  virtual void onEnter() {}

  // Executed when the UI manager is leaving this screen.
  virtual void onExit() {}

  // Invoke the enter callback. Executes generic enter handling along with the
  // callback.
  void enter() {
    cursorLine = 0;
    scrollOffset = 0;
    activeWidget = nullptr;
    onEnter();
  }

  // Invoke the exit callback. Executes generic exit handling along with the
  // callback.
  void exit() { onExit(); };

  // The UI manager passes events to each screen to handle. By default, the
  // events `UIEvent::Up` and `UIEvent::Down` are used for scrolling per line.
  // However, some widgets support focus states. For these widgets, clicking at
  // the focused widget will select it, and all events wil be passed on to the
  // widget, until it releases the selection. `UIEvent::Click` is always passed
  // through to the widget.
  virtual void handleEvent(UIEvent event);

  // The cursor on the left edge of the screen is enabled by default. It can be
  // disabled, which makes sense for loading screens or screens with no
  // interaction. Note that this does not remove the gutter, only disable the
  // cursor rendering.
  void disableCursor(bool state) { cursorDisabled = state; };

  // By default, a screen can scroll using the cursor to allow viewing more than
  // two lines at a time. For static screens like a loading screen, this can be
  // disabled.
  void disableScroll(bool state) { scrollDisabled = state; };

  // Navigators are required for any screen transitions. The navigator is passed
  // to the widget, which can use it to change the currently active screen.
  void setNavigator(Navigator* navigator) { this->navigator = navigator; };

  // Every time the UI manager is updated, the currently active screen is also
  // updated. Note that the UI manager might trigger updates multiple times per
  // loop. To perform time-sensitive operations, use delta-time-like intervals
  // or debounce the relevant method calls.
  virtual void update(uint32_t now) {}

  // Iterates over each widget, drawing its content at a offset to avoid
  // overwriting previous widgets.
  void draw();

  // The id of this screen.
  ScreenID id = ScreenID::None;

  // The parent of this screen.
  ScreenID parent = ScreenID::None;

 protected:
  // Append a widget to the list of widgets. Does nothing if the limit has been
  // hit. Widgets must be inserted in order, and cannot be removed once
  // inserted.
  void addWidget(Widget* widget);

  // Calculate and return the maximum height of all the widgets within the
  // screen.
  uint8_t totalHeight();

  // Calculate the highest scrollable line. This is usually the screen height
  // corrected for display height offset. This allows the scrolling to prevent
  // the content from going off-screen.
  uint8_t maxScroll();

  // Get the widget starting at a given line. This can be used as a linked-list,
  // as the first widget will always be at line 0, and its height can be added
  // to a counter to get the starting of the next widget, and so on.
  Widget* widgetStartingAt(uint8_t line);

  // Get the neraest widget relative to the line. The search only looks
  // backwards and not forwards. Useful to find which widget the cursor is
  // currently in to manage focus.
  Widget* getNearestWidget(uint8_t line);

  // Get the line at which the target widget starts at. Used to calculate the
  // offset to snap the widget to the center of the viewport.
  uint8_t widgetStartLine(Widget* target);

  // Data
  Widget* widgets[MAX_WIDGETS];
  uint8_t widgetCount = 0;
  uint8_t cursorLine = 0;
  uint8_t scrollOffset = 0;
  Widget* activeWidget = nullptr;
  Navigator* navigator = nullptr;

  // Behaviour state
  bool cursorDisabled = false;
  bool scrollDisabled = false;
};
