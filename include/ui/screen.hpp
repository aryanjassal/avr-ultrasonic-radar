#pragma once

#include <stdint.h>

#include "ui/event.hpp"
#include "ui/widget.hpp"

class Screen {
 public:
  static constexpr uint8_t MAX_WIDGETS = 16;

  virtual ~Screen() = default;

  // Executed when the UI manager is entering this screen.
  virtual void onEnter() {}

  // Executed when the UI manager is leaving this screen.
  virtual void onExit() {}

  // The UI manager passes events to each screen to handle. By default, the
  // events `UIEvent::Up` and `UIEvent::Down` are used for scrolling per line.
  // However, some widgets support focus states. For these widgets, clicking at
  // the focused widget will select it, and all events wil be passed on to the
  // widget, until it releases the selection. `UIEvent::Click` is always passed
  // through to the widget.
  virtual void handleEvent(UIEvent event);

  // Every time the UI manager is updated, the currently active screen is also
  // updated. Note that the UI manager might trigger updates multiple times per
  // loop. To perform time-sensitive operations, use delta-time-like intervals
  // or debounce the relevant method calls.
  virtual void update(uint32_t now) {}

  // Iterates over each widget, drawing its content at a offset to avoid
  // overwriting previous widgets.
  void draw();

 protected:
  // Append a widget to the list of widgets. Does nothing if the limit has been
  // hit. Widgets must be inserted in order, and cannot be removed once
  // inserted.
  void addWidget(Widget* widget);

 private:
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

  Widget* widgets[MAX_WIDGETS];
  uint8_t widgetCount = 0;
  uint8_t scrollOffset = 0;
  Widget* activeWidget = nullptr;
};