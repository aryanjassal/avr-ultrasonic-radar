#include <avr/interrupt.h>
#include <avr/io.h>

#include "drivers/lcd.hpp"
#include "pin.hpp"
#include "timer.hpp"

int main(void) {
  Timer::init();
  LCD display(D2, D3, D4, D5, D6, D7);

  display.print("Hello world");
  while (true);
}