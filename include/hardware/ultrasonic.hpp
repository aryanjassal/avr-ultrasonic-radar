#pragma once

#include <stdint.h>

#include "drivers/pin.hpp"

enum class UltrasonicState : uint8_t { Idle, WaitingForRise, WaitingForFall };

// The ultrasonic sensor will own Timer1. This means that Timer1 cannot be
// managed anywhere else lest it mess up the ultrasonic sensor's measurements.
// Timer1 is needed because the sensor uses ACIC to calculate the delay between
// each echo pulse. Due to this, the ECHO pin is locked to D6, with a reference
// voltage at D7. As there is only a single pin capable of driving the
// ultrasonic sensor, this is a namespace instead of a class to indicate only
// one instance can exist at a time. The trigger pin can be configured to any
// available GPIO pin, howwever.
namespace Ultrasonic {
// The ECHO pin uses the ACIC and does not need to be provided separately. Owns
// Timer1; it cannot be used by other parts of the software anymore. To allow
// for stable comparator readings, the GPIO for D6 and D7 is disabled.
void init(const PinDescriptor& trigPin);

// Trigger a measurement to be taken. If the sensor is currently within a
// measurement or if the previous measurement happened too early, then the
// call will fail, indicated by a false return.
bool trigger();

// Run during the update loop of the program. Does timeout calculation to
// abort the reading if it takes too long.
void update();

// Is false if the sensor is currently in the middle of a measurement. Returns
// true once the sensor has retrieved a measurement, and will be true until
// another measurement is triggered.
bool ready();

// Get the last known distance in mm. After triggering a new measurement, use
// the `Ultrasonic::ready()` function to wait until the sensor is ready with
// the next measurement. This allows for non-blocking waiting.
uint16_t distance_mm();

// The ACIC must first capture rising edge, then falling edge. To do this, the
// edge detection is switched dynamically in the handler. This must be run
// within an ISR for ACIC handling. As interrupts are static/global protocols,
// the sensor being captured must also be static/global.
void handleCapture(uint16_t capture);
};  // namespace Ultrasonic