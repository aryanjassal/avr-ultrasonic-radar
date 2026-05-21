#pragma once

#include <stdint.h>

#include "drivers/pin.hpp"

#define MATRIX_REG_NOOP 0x00

#define MATRIX_REG_ROW0 0x01
#define MATRIX_REG_ROW1 0x02
#define MATRIX_REG_ROW2 0x03
#define MATRIX_REG_ROW3 0x04
#define MATRIX_REG_ROW4 0x05
#define MATRIX_REG_ROW5 0x06
#define MATRIX_REG_ROW6 0x07
#define MATRIX_REG_ROW7 0x08

#define MATRIX_REG_DECODE_MODE 0x09
#define MATRIX_REG_INTENSITY 0x0a
#define MATRIX_REG_SCAN_LIMIT 0x0b
#define MATRIX_REG_SHUTDOWN 0x0c
#define MATRIX_REG_DISPLAY_TEST 0x0f

#define MATRIX_SHUTDOWN_MODE 0x00
#define MATRIX_NORMAL_MODE 0x01

#define MATRIX_TEST_DISABLE 0x00
#define MATRIX_TEST_ENABLE 0x01

#define MATRIX_SCAN_ALL_ROWS 0x07

#define MATRIX_DECODE_DISABLE 0x00

enum MatrixIntensity {
  MATRIX_INTENSITY_MIN = 0x00,
  MATRIX_INTENSITY_LOW = 0x04,
  MATRIX_INTENSITY_MED = 0x08,
  MATRIX_INTENSITY_HIGH = 0x0c,
  MATRIX_INTENSITY_MAX = 0x0f
};

class Matrix {
 private:
  OutputPin DIN, CS, CLK;

  // Shifts a byte of data into the matrix display bit-by-bit.
  void _shift(uint8_t data);

  // Writes data into a specified register. As there is only a signel data-in
  // pin, each bit of the data needed to be written is shifted in. First, the
  // register is shifted in bit-by-bit, then the value is.
  void _write(uint8_t reg, uint8_t value);

 public:
  // Takes in the DIN, CS, and CLK pins to initialise the display. While
  // traditionally SPI is used to manage this display, due to a lack of
  // available pins, manual SPI is favoured instead.
  Matrix(const PinDescriptor& din, const PinDescriptor& cs,
         const PinDescriptor& clk);

  // Clears the display by setting each row to 0b00000000.
  void clear();

  // Each pixel is a bitfield per forw, so a single `uint8_t` can be used to
  // write pixels to an entire row.
  void setRow(uint8_t row, uint8_t value);

  // Controls the brightness of the pixels.
  void brightness(MatrixIntensity intensity);

  // Controls if the board is enabled or not, as it offers its own power switch.
  void power(bool enabled);

  // Test for dead pixels by bypassing all programming and directly powering
  // each pixel.
  void test(bool enabled);
};