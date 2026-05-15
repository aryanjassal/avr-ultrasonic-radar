#include "hardware/ultrasonic.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "drivers/timer.hpp"
#include "state.hpp"

static OutputPin* trig = nullptr;

volatile UltrasonicState ultrasonicState = UltrasonicState::Idle;

volatile bool measurementReady = false;

volatile uint16_t captureStart = 0;
volatile uint16_t captureEnd = 0;

volatile uint16_t lastDistance = 0;

uint32_t measurementStart = 0;
uint32_t lastPing = 0;

void Ultrasonic::init(const PinDescriptor& trigPin) {
  // Set trigger pin
  static OutputPin trigger(trigPin);
  trig = &trigger;

  // Timer is not owned by the sensor. It must use the already-present prescaler
  // settings along with any other modes. The sensor should only modify the
  // relevant bit flags and leave the rest as-is.

  ACSR |= (1 << ACIC);                   // Analog comparator input capture
  TCCR1B |= (1 << ICES1);                // Capture rising edge initially
  TIMSK1 |= (1 << ICIE1);                // Enable input capture interrupt
  DIDR1 |= (1 << AIN0D) | (1 << AIN1D);  // Disable GPIO

  // Ensure interrupts are enabled
  sei();
}

bool Ultrasonic::trigger() {
  // Sensor can't be triggered if it's busy or last ping was too recent.
  if (ultrasonicState != UltrasonicState::Idle) { return false; }
  if (millis() - lastPing < 60) return false;
  if (trig == nullptr) return false;

  // Prepare state for another measurement
  measurementReady = false;
  ultrasonicState = UltrasonicState::WaitingForRise;
  measurementStart = micros();
  lastPing = millis();

  TCCR1B |= (1 << ICES1);  // Rising edge first
  TIFR1 |= (1 << ICF1);    // Clear pending capture flag

  // Pulse trigger
  trig->high();
  delayMicroseconds(10);
  trig->low();

  return true;
}

void Ultrasonic::update() {
  // Timeout management
  if (trig == nullptr) return;
  if (ultrasonicState != UltrasonicState::Idle &&
      micros() - measurementStart > 30000) {
    ultrasonicState = UltrasonicState::Idle;
    // So `Ultrasonic::ready()` doesn't stall on a timeout.
    measurementReady = true;
    lastDistance = 0;
    TCCR1B |= (1 << ICES1);  // Re-arm rising edge
  }
}

void Ultrasonic::handleCapture(uint16_t capture) {
  if (ultrasonicState == UltrasonicState::WaitingForRise) {
    captureStart = capture;
    ultrasonicState = UltrasonicState::WaitingForFall;
    TCCR1B &= ~(1 << ICES1);  // Switch to falling edge
    return;
  }

  if (ultrasonicState == UltrasonicState::WaitingForFall) {
    captureEnd = capture;
    uint16_t duration = captureEnd - captureStart;

    // Timer tick must be converted depending on Timer1 prescaler.
    lastDistance =
        (duration * 5) / 58;  // TODO: dynamically infer this based on the timer
                              // state or require a parameter for this
    measurementReady = true;
    ultrasonicState = UltrasonicState::Idle;

    // Rearm rising edge
    TCCR1B |= (1 << ICES1);
  }
}

bool Ultrasonic::ready() { return measurementReady; }

uint16_t Ultrasonic::distance_mm() {
  // Clamp values between maximum and minimum distance
  if (lastDistance > state.maxDistance) return state.maxDistance;
  if (lastDistance < state.minDistance) return state.minDistance;
  return lastDistance;
}

ISR(TIMER1_CAPT_vect) { Ultrasonic::handleCapture(ICR1); }