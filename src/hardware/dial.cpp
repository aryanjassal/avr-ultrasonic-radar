#include "dial.hpp"

#include <avr/io.h>

#define ENC_PORT PORTB
#define ENC_PIN PINB
#define ENC_DDR DDRB

#define ENC_CLK PB0
#define ENC_DT PB2
#define ENC_SW PB3

static uint8_t prev_state = 0;

void Dial::init() {
  // Inputs
  ENC_DDR &= ~((1 << ENC_CLK) | (1 << ENC_DT) | (1 << ENC_SW));

  // Pullups
  ENC_PORT |= (1 << ENC_CLK) | (1 << ENC_DT) | (1 << ENC_SW);

  uint8_t clk = (ENC_PIN & (1 << ENC_CLK)) ? 1 : 0;

  uint8_t dt = (ENC_PIN & (1 << ENC_DT)) ? 1 : 0;

  prev_state = (clk << 1) | dt;
}

int8_t Dial::poll() {
  uint8_t clk = (ENC_PIN & (1 << ENC_CLK)) ? 1 : 0;

  uint8_t dt = (ENC_PIN & (1 << ENC_DT)) ? 1 : 0;

  uint8_t state = (clk << 1) | dt;

  int8_t movement = 0;

  // State transition table
  switch ((prev_state << 2) | state) {
  // Clockwise
  case 0b0001:
  case 0b0111:
  case 0b1110:
  case 0b1000:
    movement = +1;
    break;

  // Counterclockwise
  case 0b0010:
  case 0b1011:
  case 0b1101:
  case 0b0100:
    movement = -1;
    break;
  }

  prev_state = state;

  return movement;
}

uint8_t Dial::pressed() { return !(ENC_PIN & (1 << ENC_SW)); }