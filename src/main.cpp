#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#include "drivers/display.hpp"
#include "drivers/pin.hpp"
#include "drivers/timer.hpp"
#include "hardware/dial.hpp"
#include "hardware/lcd.hpp"
#include "ui/manager.hpp"
#include "ui/screen.hpp"

class HelloScreen : public Screen {
 public:
  void handleEvent(UIEvent event) override {
    if (event == UIEvent::Down) counter++;
    if (event == UIEvent::Up && counter > 0) counter--;
  }

  void draw() override {
    Display::drawText(0, 0, "Hello world");

    Display::drawText(0, 1, "Value:");

    char buf[5];
    itoa(counter, buf, 10);
    Display::drawText(7, 1, buf);
  }

 private:
  uint8_t counter = 0;
};

int main(void) {
  Timer::init();
  Dial::init();
  LCD display(D8, D9, D10, D11, D12, D13);
  Display::init(&display);

  HelloScreen hello;
  UIManager ui;

  ui.init(&hello);

  while (true) {
    UIEvent event = UIEvent::None;

    int8_t delta = Dial::poll();
    if (delta > 0)
      event = UIEvent::Up;
    else if (delta < 0)
      event = UIEvent::Down;

    ui.update(millis(), event);
  }
}