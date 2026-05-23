#pragma once

#include <stdint.h>
#include "drivers/pin.hpp"

namespace RadarController {
// Initialise the servo to drive the radar head and the ultrasonic sensor to
// detect distances. Only the ultrasonic sensor's TRIG pin can be set to any
// GPIO pin, all other pins are locked.
void init(const PinDescriptor& ultrasonicTrigger);

// Generates the stopping points for the servo motor. This avoids the load of
// computing the same data per frame. This is not automatically called during
// intialisation and must be called manually.
void generateStoppingPoints();

// Stops executing the main scanning loop. Once the system has been stopped,
// specific angles can be manually written to the servo, bypassing internal
// state. This means no callbacks will be executed.
void stop();

// Start executing the main loop of the system. The callback will start
// executing with each angle-distance reading obtained.
void start();

// Manually write a specific angle to the servo. Only has an impact when the
// system is stopped.
void writeAngleToServo(uint8_t angle);

// Updates internal state without blocking.
void update();

// Sets the callback for whenever an angle-distance pair is ready.
void setCallback(void (*callback)(uint8_t angle, uint16_t distance,
                                  uint8_t pointIndex));

// Get the stopping points for the radar sweep.
uint8_t* getStoppingPoints();

// Get the number of stopping points for the radar sweep.
uint8_t getStoppingPointCount();

};  // namespace RadarController