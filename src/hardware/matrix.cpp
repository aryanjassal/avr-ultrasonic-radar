#include "hardware/matrix.hpp"
#include "drivers/timer.hpp"

Matrix::Matrix(const PinDescriptor& din, const PinDescriptor& cs,
               const PinDescriptor& clk)
    : DIN(din), CS(cs), CLK(clk) {
  // Set the default pin status
  CS.high();
  CLK.low();
  DIN.low();
  delayMicroseconds(10);

  // Initialise the panel
  _write(MATRIX_REG_NOOP, 0x00);
  power(false);
  _write(MATRIX_REG_DECODE_MODE, MATRIX_DECODE_DISABLE);
  _write(MATRIX_REG_SCAN_LIMIT, MATRIX_SCAN_ALL_ROWS);
  test(false);
  brightness(MATRIX_INTENSITY_LOW);
  clear();
}

// Write data bit-by-bit through DIN
void Matrix::_shift(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    CLK.low();

    // clang-format off
    if (data & (1 << (7 - i))) DIN.high();
    else DIN.low();
    // clang-format on

    delayMicroseconds(1);
    CLK.high();
    delayMicroseconds(1);
  }
}

// Write some data to a specified register
void Matrix::_write(uint8_t reg, uint8_t value) {
  CS.low();
  _shift(reg);
  _shift(value);
  CS.high();
  delayMicroseconds(5);
}

// Clear the display
void Matrix::clear() {
  for (uint8_t row = 0; row < 8; row++) { setRow(row, 0x00); }
}

// Set pixel state in one row
void Matrix::setRow(uint8_t row, uint8_t value) {
  if (row >= 8) return;
  _write(MATRIX_REG_ROW0 + row, value);
}

// Set brightness
void Matrix::brightness(MatrixIntensity intensity) {
  _write(MATRIX_REG_INTENSITY, intensity);
}

// Control panel power
void Matrix::power(bool enabled) {
  _write(MATRIX_REG_SHUTDOWN,
         enabled ? MATRIX_SHUTDOWN_MODE : MATRIX_NORMAL_MODE);
}

// Toggle all lights on regardless of programming
void Matrix::test(bool enabled) {
  _write(MATRIX_REG_DISPLAY_TEST,
         enabled ? MATRIX_TEST_ENABLE : MATRIX_TEST_DISABLE);
}