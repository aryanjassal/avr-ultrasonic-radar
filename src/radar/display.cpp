#include "radar/display.hpp"

#include "drivers/matrix.hpp"
#include "radar/controller.hpp"
#include "state.hpp"

static int8_t columns[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
static int8_t sweepColumn = -1;

static int8_t pointToColumn(uint8_t pointIndex) {
  return pointIndex + ((8 - RadarController::getStoppingPointCount()) / 2);
}

static int8_t distanceToRow(uint16_t distance) {
  uint16_t min = state.minDistance;
  uint16_t max = state.maxDistance;
  if (max <= min) return -1;
  if (distance < min) distance = min;
  if (distance > max) distance = max;
  uint8_t radius = ((uint32_t)(distance - min) * 6) / (max - min);
  return 6 - radius;
}

void RadarDisplay::clear() {
  for (uint8_t i = 0; i < 8; i++) { columns[i] = -1; }
}

void RadarDisplay::plot(uint8_t pointIndex, uint16_t distance) {
  int8_t column = pointToColumn(pointIndex);
  if (column < 0) return;
  columns[column] = distanceToRow(distance);
  sweepColumn = column;
}

void RadarDisplay::render() {
  MatrixDisplay::clearBuffer();

  // Disable matrix rendering if USART rendering is enabled.
  if (state.usartRendering) return;

  // Occupancy columns
  for (uint8_t x = 0; x < 8; x++) {
    int8_t hitY = columns[x];
    if (hitY < 0) continue;  // Unknown / no data
    // Fill blocked region from obstacle outward/farther away.
    for (uint8_t y = 0; y <= hitY; y++) { MatrixDisplay::setPixel(x, y, true); }
  }

  // Radar origin if still configuring
  if (!state.ready) {
    MatrixDisplay::setPixel(3, 7, true);
    MatrixDisplay::setPixel(4, 7, true);
  }

  // Sweep indicator
  if (sweepColumn >= 0) { MatrixDisplay::setPixel(sweepColumn, 7, true); }
  MatrixDisplay::render();
}