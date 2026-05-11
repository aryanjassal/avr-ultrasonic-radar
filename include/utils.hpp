#pragma once

#include <stdint.h>

#define PHOTOCELL_MIN 240
#define PHOTOCELL_MAX 700

#define LED_PWM_MIN 15
#define LED_PWM_MAX 255

// Inverse mapping for higher brightness -> lower PWM.
uint8_t brightnessToPwm(uint16_t brightness);
