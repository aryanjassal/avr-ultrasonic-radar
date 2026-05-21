#pragma once

#include <stdint.h>

namespace RadarDisplay {

// Clear the radar display
void clear();

// Plot an point-distance pair on the screen. Note that the point index is
// different than angle.
void plot(uint8_t pointIndex, uint16_t distance);

// Render the measurements
void render();

}  // namespace RadarDisplay