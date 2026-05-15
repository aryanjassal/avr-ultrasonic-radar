#include <avr/interrupt.h>
#include <avr/io.h>

#include "application/calibrate.hpp"
#include "drivers/display.hpp"
#include "drivers/pin.hpp"
#include "drivers/timer.hpp"
#include "hardware/dial.hpp"
#include "hardware/lcd.hpp"
#include "ui/manager.hpp"

int main(void) {
  Timer::init();
  Dial::init();
  LCD display(D8, A1, A2, A3, A4, A5);
  Display::init(&display);

  UIManager ui;
  CalibrationScreen calibrate;

  ui.init(&calibrate);

  while (true) {
    UIEvent event = UIEvent::None;

    // Handle state change. Each dial state is exclusive, and click takes
    // precedence to avoid accidental scrolling during clicking.
    int8_t delta = Dial::poll();
    if (Dial::clicked())
      event = UIEvent::Click;
    else if (delta > 0)
      event = UIEvent::Down;
    else if (delta < 0)
      event = UIEvent::Up;

    ui.update(event);
  }
}