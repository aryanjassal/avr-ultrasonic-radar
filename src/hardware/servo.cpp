#include "hardware/servo.hpp"

#include <avr/io.h>
#include <stdint.h>

void Servo::init() {
  DDRB |= (1 << PB1);
  TCCR1A = (1 << COM1A1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  ICR1 = 40000;
  OCR1A = 3000;
}

void Servo::write_angle(uint8_t angle) {
  // Unsigned char data type means no negative values
  if (angle > 180) angle = 180;
  // Calculate pulse duration
  uint16_t pulse_us =
      SERVO_MIN_US + ((uint32_t)angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;
  OCR1A = pulse_us * 2;
}