#pragma once

#include <stdint.h>

enum class ServoState : uint8_t { Idle, Moving };

namespace Servo {
// The servo owns Timer1 configuration. The PWM pin used for this is D10.
// Configuration like setting up the prescaler happens with the servo
// initialisation.
void init();

// Target angle in degrees. Must be between 0 and 180.
void write_angle(uint8_t angle);

// Current commanded angle.
uint8_t angle();

// Update movement state.
void update();

// Whether servo is currently moving.
bool moving();

}  // namespace Servo