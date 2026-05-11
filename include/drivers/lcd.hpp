#pragma once

#include "pin.hpp"

#define LCD_OP_CLEAR_BASE 0x01
#define LCD_OP_HOME_BASE 0x02

#define LCD_OP_ENTRY_BASE 0x04
#define LCD_OP_ENTRY_INC 0x02
#define LCD_OP_ENTRY_SHIFT 0x01

#define LCD_OP_DISPLAY_BASE 0x08
#define LCD_OP_DISPLAY_ON 0x04
#define LCD_OP_CURSOR_ON 0x02
#define LCD_OP_BLINK_ON 0x01

#define LCD_OP_SHIFT_BASE 0x10
#define LCD_OP_SHIFT_DISPLAY 0x08
#define LCD_OP_SHIFT_RIGHT 0x04

#define LCD_OP_FUNCTION_BASE 0x20
#define LCD_OP_8BIT_MODE 0x10
#define LCD_OP_2_LINE 0x08
#define LCD_OP_5x10_FONT 0x04

enum LCD_CMD {
  // Basic sequences
  LCD_CMD_CLEAR = 0x01,
  LCD_CMD_HOME = 0x02,

  // Cursor manipulation
  LCD_CMD_CURSOR_DEC = 0x04,
  LCD_CMD_CURSOR_DEC_SHIFT = 0x05,
  LCD_CMD_CURSOR_INC = 0x06,
  LCD_CMD_CURSOR_INC_SHIFT = 0x07,

  // Display options. Best manipulated with bitfields.
  LCD_CMD_DISP_CUR_BLINK_OFF = 0x08,
  LCD_CMD_DISP_CUR_OFF_BLINK_ON = 0x09,
  LCD_CMD_DISP_BLINK_OFF_CUR_ON = 0x0a,
  LCD_CMD_DISP_OFF_CUR_BLINK_ON = 0x0b,
  LCD_CMD_CUR_BLINK_OFF_DISP_ON = 0x0c,
  LCD_CMD_CUR_OFF_DISP_BLINK_ON = 0x0d,
  LCD_CMD_BLINK_OFF_DISP_CUR_ON = 0x0e,
  LCD_CMD_DISP_CUR_BLINK_ON = 0x0f,

  // Cursor/display shift
  LCD_CMD_CURSOR_SHIFT_LEFT = 0x10,
  LCD_CMD_CURSOR_SHIFT_RIGHT = 0x14,
  LCD_CMD_DISPLAY_SHIFT_LEFT = 0x18,
  LCD_CMD_DISPLAY_SHIFT_RIGHT = 0x1c,

  // Function set
  LCD_CMD_4BIT_1LINE_5x8 = 0x20,
  LCD_CMD_4BIT_1LINE_5x10 = 0x24,
  LCD_CMD_4BIT_2LINE_5x8 = 0x28,
  LCD_CMD_4BIT_2LINE_5x10 = 0x2c,

  LCD_CMD_8BIT_1LINE_5x8 = 0x30,
  LCD_CMD_8BIT_1LINE_5x10 = 0x34,
  LCD_CMD_8BIT_2LINE_5x8 = 0x38,
  LCD_CMD_8BIT_2LINE_5x10 = 0x3c
};

class LCD {
 private:
  // All the pins required to control the display.
  OutputPin RS, E, D4, D5, D6, D7;

  // Write a 4-bit data segment to the display.
  void _write_nibble(uint8_t nibble);

  // Send data to the display with register selection.
  void _send(uint8_t value, uint8_t rs);

 public:
  // The LCD is a 16-pin version. There are 6 main data channels controllable by
  // the MCU: RS, E, and D4-D7. There are actually 8 data pins, but to conserve
  // pins, 4-bit nibbles can be sent at a time, reducing the data pins from 8
  // to 4.
  LCD(const PinDescriptor& registerSelect, const PinDescriptor& enable,
      const PinDescriptor& D4, const PinDescriptor& D5, const PinDescriptor& D6,
      const PinDescriptor& D7);

  // Write data to the display. Useful for actually rendering text.
  void print(uint8_t data);

  // Print string sequences instead of printing each byte individually.
  void print(const char* str);

  // Send control sequences to the display.
  void command(LCD_CMD command);
};