#include "utils.hpp"

uint8_t brightnessToPwm(uint16_t brightness) {
  if (brightness < PHOTOCELL_MIN) {
    return LED_PWM_MAX;
  }
  if (brightness > PHOTOCELL_MAX) {
    return LED_PWM_MIN;
  }

  float normalised =
      (brightness - PHOTOCELL_MIN) / (float)(PHOTOCELL_MAX - PHOTOCELL_MIN);
  uint16_t pwmRange = LED_PWM_MAX - LED_PWM_MIN;
  return LED_PWM_MAX - (uint8_t)(normalised * pwmRange);
}