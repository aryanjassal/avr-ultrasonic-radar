#include "ultrasonic.hpp"
#include "timer.hpp"

Ultrasonic::Ultrasonic(const PinDescriptor &trigPin,
                       const PinDescriptor &echoPin)
    : trig(trigPin), echo(echoPin) {}

void Ultrasonic::trigger() {
  // Rate-limit pinging
  if (millis() - lastPing < 60)
    return;

  // Send trigger pulse (10 µs)
  lastPing = millis();
  trig.high();
  uint32_t start = micros();
  while (micros() - start < 10)
    ;
  trig.low();

  // Wait for echo HIGH
  uint32_t timeout = micros();
  while (!echo.readRaw()) {
    if (micros() - timeout > 30000) {
      lastDistance = 0;
      measurementReady = false;
      return;
    }
  }

  // Echo pulse started
  uint32_t echoStart = micros();

  // Wait for echo LOW
  while (echo.readRaw()) {
    if (micros() - echoStart > 30000) {
      lastDistance = 0;
      measurementReady = false;
      return;
    }
  }

  uint32_t echoEnd = micros();
  uint32_t duration = echoEnd - echoStart;

  // distance_mm = duration_us * 10 / 58
  lastDistance = (duration * 10) / 58;
  measurementReady = true;
}

bool Ultrasonic::ready() const { return measurementReady; }

uint16_t Ultrasonic::distance_mm() const { return lastDistance; }