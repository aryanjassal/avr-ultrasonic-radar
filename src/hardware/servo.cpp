#include "hardware/servo.hpp"

#include <avr/io.h>
#include "drivers/timer.hpp"

#define SERVO_PIN PB2

#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500

// Timer1:
// Prescaler 8
// Tick = 0.5us
//
// 20ms frame:
// 20000us / 0.5us = 40000

static volatile uint32_t movementStarted = 0;
static volatile uint8_t currentAngle = 90;
static volatile uint8_t targetAngle = 90;

static volatile ServoState servoState = ServoState::Idle;

void Servo::init() {
  // OC1B output
  DDRB |= (1 << SERVO_PIN);

  // Fast PWM
  // TOP = OCR1A
  //
  // WGM:
  // 1111 -> Fast PWM OCR1A TOP
  TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);

  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

  // 20ms frame
  OCR1A = 40000;

  // Default center pulse
  OCR1B = 3000;
}

void Servo::write_angle(uint8_t angle) {
  if (angle > SERVO_MAX_ANGLE) { angle = SERVO_MAX_ANGLE; }
  if (targetAngle == angle) { return; }

  targetAngle = angle;
  servoState = ServoState::Moving;
  movementStarted = millis();

  uint16_t pulse_us =
      SERVO_MIN_US + ((uint32_t)angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;

  OCR1B = pulse_us * 2;
}

void Servo::update() {
  if (servoState == ServoState::Moving) {
    // Assuming 50ms per movement event. Should be fine for the small increments
    // for the radar sweep.
    if (millis() - movementStarted >= 50) {
      currentAngle = targetAngle;
      servoState = ServoState::Idle;
    }
  }
}

uint8_t Servo::angle() { return currentAngle; }

bool Servo::moving() { return servoState == ServoState::Moving; }
