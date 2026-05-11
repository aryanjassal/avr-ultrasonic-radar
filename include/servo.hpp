#pragma once

#include <stdint.h>

#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500

class Servo {
public:
  // Servo is initialised on Digital 9 pin of the board.
  static void init();

  // Angle must be between 0 and 180 degrees.
  static void write_angle(uint8_t angle);
};