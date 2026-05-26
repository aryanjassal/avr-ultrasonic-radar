#include <avr/interrupt.h>
#include <avr/io.h>

#include "application/alarm.hpp"
#include "application/calibrate.hpp"
#include "application/loading.hpp"
#include "application/menu.hpp"
#include "application/screens.hpp"
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

static UIManager ui;
static LoadingScreen loading;
static CalibrationScreen calibrate;
static MenuScreen menu;
static AlarmScreen alarm;
volatile bool receivedToggle = false;

static void usartRxHandler() {
  // Read and discard UDR0
  (void)UDR0;

  // Actual logic to toggle USART mode
  if (state.usartRendering == true) return;
  state.usartRendering = true;
  receivedToggle = true;
}

int main(void) {
  Timer::init();
  Analog::init();
  Dial::init();
  LCD display(D8, A1, A2, A3, A4, A5);
  LCDDisplay::init(&display);

  // Initialise the loading screen while the system prepares. The loading screen
  // is displayed before the main loop starts, so it is impossible for this
  // screen to get events.
  ui.init();
  ui.registerScreen(&loading);
  ui.navigate(ScreenID::Loading);

  ui.registerScreen(&calibrate);
  ui.registerScreen(&menu);
  ui.registerScreen(&alarm);

  // Set up sensors and peripherals
  USART::init(57600);
  RadarController::init(D5);
  RadarController::setCallback(
      [](uint8_t angle, uint16_t distance, uint8_t pointIndex) {
        if (state.usartRendering) {
          USART::print("$A");
          USART::print_uint32(angle);
          USART::print(",D");
          USART::print_uint32(distance);
          USART::print(",I");
          USART::print_uint32(pointIndex);
          USART::println();
        } else {
          RadarDisplay::plot(pointIndex, distance);
        }
      });

  // Setup the matrix display
  Matrix matrix(D9, D12, D13);
  MatrixDisplay::init(&matrix);
  MatrixDisplay::setOrientation(MatrixDisplay::Orientation::Deg180);

  Sound soundSensor(A0);
  Buzzer buzzer(D11);

  // Swap to actual program screen
  ui.navigate(ScreenID::Calibration);

  // The user can send any message through USART, which will disable local
  // rendering and send the data to USART for remote rendering instead. This
  // will also enhance the scan resolution to give a better output to the
  // high-res display.
  USART::installRx(usartRxHandler);
  USART::println("Type anything to enable USART rendering.");

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

    if (receivedToggle) {
      receivedToggle = false;
      USART::println("USART rendering enabled.");
      printCalibrationMetrics();
    }

    // Send data to USART if requested
    if (state.sendAmplitude) {
      USART::print(">amplitude:");
      USART::print_uint32(soundSensor.amplitude());
      USART::println();
    }

    // Update alarm state. Make sure to clear spike even if the alarm cannot be
    // triggered.
    if (soundSensor.spike()) {
      soundSensor.clearSpike();
      if (state.mode == RadarMode::Alert && !state.alarmEnabled) {
        state.alarmEnabled = true;
        RadarController::stop();
        buzzer.start();

        // Navigate to alert screen
        alarm.parent = ui.getCurrentScreen()->id;
        ui.navigate(ScreenID::Alarm);
      }
    }

    // Suppress alarm. Radar will be started by alarm screen. If there was a
    // spike while alarm was enabled, then ignore it.
    if (!state.alarmEnabled && buzzer.active()) {
      buzzer.stop();
      soundSensor.clearSpike();
    }

    // Update systems
    ui.update(event);
    soundSensor.update();
    RadarController::update();
    RadarDisplay::render();
  }
}
