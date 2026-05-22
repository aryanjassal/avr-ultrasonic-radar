#include <avr/interrupt.h>
#include <avr/io.h>

#include "application/calibrate.hpp"
#include "application/loading.hpp"
#include "application/menu.hpp"
#include "application/screens.hpp"
#include "drivers/adc.hpp"
#include "drivers/lcd.hpp"
#include "drivers/matrix.hpp"
#include "drivers/pin.hpp"
#include "drivers/sound.hpp"
#include "drivers/timer.hpp"
#include "drivers/usart.hpp"
#include "hardware/buzzer.hpp"
#include "hardware/dial.hpp"
#include "hardware/lcd.hpp"
#include "hardware/matrix.hpp"
#include "radar/controller.hpp"
#include "radar/display.hpp"
#include "state.hpp"
#include "ui/manager.hpp"

int main(void) {
  Timer::init();
  Adc::init();
  Dial::init();
  LCD display(D8, A1, A2, A3, A4, A5);
  LCDDisplay::init(&display);

  // Initialise the loading screen while the system prepares. The loading screen
  // is displayed before the main loop starts, so it is impossible for this
  // screen to get events.
  UIManager ui;
  LoadingScreen loading;
  ui.init();
  ui.registerScreen(ScreenID::Loading, &loading);
  ui.navigate(ScreenID::Loading);

  // Set up alternate screens
  CalibrationScreen calibrate;
  MenuScreen menu;

  ui.registerScreen(ScreenID::Calibration, &calibrate);
  ui.registerScreen(ScreenID::Menu, &menu);

  // Set up sensors and peripherals
  USART::init(57600);
  RadarController::init(D5);
  RadarController::setCallback(
      [](uint8_t angle, uint16_t distance, uint8_t pointIndex) {
        RadarDisplay::plot(pointIndex, distance);
      });

  // Setup the matrix display
  Matrix matrix(D9, D12, D13);
  MatrixDisplay::init(&matrix);
  MatrixDisplay::setOrientation(MatrixDisplay::Orientation::Deg180);

  Sound soundSensor(A0);
  Buzzer buzzer(D11);

  // Swap to actual program screen
  ui.navigate(ScreenID::Calibration);

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

    USART::print(">amplitude:");
    USART::print_uint32(soundSensor.amplitude());
    USART::println();

    // Update alarm state
    if (soundSensor.spike()) {
      soundSensor.clearSpike();
      state.alarmEnabled = true;
    }

    // Trigger alarm
    if (state.alarmEnabled && !buzzer.active()) {
      RadarController::stop();
      buzzer.tone(2000);
    } else if (!state.alarmEnabled && buzzer.active()) {
      buzzer.stop();
      RadarController::start();
    }

    // Update systems
    ui.update(event);
    soundSensor.update();
    RadarController::update();
    RadarDisplay::render();
  }
}