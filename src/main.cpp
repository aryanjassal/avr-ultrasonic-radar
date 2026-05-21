#include <avr/interrupt.h>
#include <avr/io.h>

#include "application/calibrate.hpp"
#include "application/loading.hpp"
#include "drivers/lcd.hpp"
#include "drivers/matrix.hpp"
#include "drivers/pin.hpp"
#include "drivers/radar.hpp"
#include "drivers/timer.hpp"
#include "drivers/usart.hpp"
#include "hardware/dial.hpp"
#include "hardware/lcd.hpp"
#include "hardware/matrix.hpp"
#include "ui/manager.hpp"

int main(void) {
  Timer::init();
  Dial::init();
  LCD display(D8, A1, A2, A3, A4, A5);
  LCDDisplay::init(&display);

  // Initialise the loading screen while the system prepares. The loading screen
  // is displayed before the main loop starts, so it is impossible for this
  // screen to get events.
  UIManager ui;
  LoadingScreen loading;
  ui.init(&loading);

  // Set up alternate screens
  CalibrationScreen calibrate;

  // Set up sensors and peripherals
  USART::init(57600);
  RadarController::init(D5);
  RadarController::setCallback([](uint8_t angle, uint16_t distance) {
    USART::print(">angle:");
    USART::print_int32(angle);
    USART::println();
    USART::print(">distance:");
    USART::print_int32(distance);
    USART::println();
  });

  // Setup the matrix display
  Matrix matrix(D11, D12, D13);
  MatrixDisplay::init(&matrix);
  MatrixDisplay::setOrientation(MatrixDisplay::Orientation::Deg270);

  // Swap to actual program screen
  ui.setScreen(&calibrate);

  int row = 0, col = 0;
  uint32_t lastUpdatedMs = 0;

  while (true) {
    // Handle state change. Each dial state is exclusive, and click takes
    // precedence to avoid accidental scrolling during clicking.
    UIEvent event = UIEvent::None;
    int8_t delta = Dial::poll();
    // clang-format off
    if (Dial::clicked()) event = UIEvent::Click;
    else if (delta > 0) event = UIEvent::Down;
    else if (delta < 0) event = UIEvent::Up;
    // clang-format on

    if (millis() - lastUpdatedMs > 100) {
      lastUpdatedMs = millis();
      MatrixDisplay::clear();
      col++;
      if (col > 7) {
        if (row == 7) {
          row = 0;
        } else {
          row++;
        }
        col = 0;
      }
      MatrixDisplay::setPixel(row, col, true);
    }

    // Update systems
    ui.update(event);
    MatrixDisplay::render();
    RadarController::update();
  }
}