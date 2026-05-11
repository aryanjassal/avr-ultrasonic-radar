#include "hardware/lcd.hpp"

#include "drivers/pin.hpp"
#include "drivers/timer.hpp"

static const uint8_t row_offsets[] = {0x00, 0x40};

LCD::LCD(const PinDescriptor& registerSelect, const PinDescriptor& enable,
         const PinDescriptor& D4, const PinDescriptor& D5,
         const PinDescriptor& D6, const PinDescriptor& D7)
    : RS(registerSelect), E(enable), D4(D4), D5(D5), D6(D6), D7(D7) {
  // The panel needs 50ms to initialise
  delay(50);

  // Initialise 4-bit mode
  _write_nibble(0x03);
  delay(5);
  _write_nibble(0x03);
  delayMicroseconds(150);
  _write_nibble(0x03);
  _write_nibble(0x02);

  // Initialise display
  command(LCD_CMD_4BIT_2LINE_5x8);
  command(LCD_CMD_CUR_BLINK_OFF_DISP_ON);
  command(LCD_CMD_CURSOR_INC);
  command(LCD_CMD_CLEAR);
  delay(2);
}

void LCD::command(LCD_CMD command) { _send(command, 0); }

void LCD::_send(uint8_t value, uint8_t rs) {
  if (rs)
    RS.high();
  else
    RS.low();

  // High nibble, then low nibble
  _write_nibble(value >> 4);
  _write_nibble(value & 0x0f);
  delayMicroseconds(50);
}

void LCD::_write_nibble(uint8_t nibble) {
  // Reset data pins
  D4.low();
  D5.low();
  D6.low();
  D7.low();

  // Set pins corresponding to enabled nibble
  if (nibble & 0x01) D4.high();
  if (nibble & 0x02) D5.high();
  if (nibble & 0x04) D6.high();
  if (nibble & 0x08) D7.high();

  // Pulse the enable pin
  E.high();
  delayMicroseconds(1);
  E.low();
  delayMicroseconds(100);
}

void LCD::print(const char* str) {
  while (*str) print(*str++);
}

void LCD::print(uint8_t byte) { _send(byte, 1); }

void LCD::setCursor(uint8_t x, uint8_t y) {
  if (x >= 16) x = 15;
  if (y >= 2) y = 1;
  command((LCD_CMD)(0x80 | (row_offsets[y] + x)));
}

void LCD::clear() {
  command(LCD_CMD_CLEAR);
  delay(2);
}

void LCD::home() {
  command(LCD_CMD_HOME);
  delay(2);
}