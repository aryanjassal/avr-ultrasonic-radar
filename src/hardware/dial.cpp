#include "hardware/dial.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>

#define ENC_PORT PORTD
#define ENC_PIN PIND
#define ENC_DDR DDRD

#define ENC_CLK PD2  // INT0
#define ENC_DT PD3   // INT1
#define ENC_SW PD4

// Previous quadrature state
static volatile uint8_t prev_state = 0;

// Accumulated transition count
static volatile int8_t accumulator = 0;

// Logical detent movement
static volatile int8_t pending_movement = 0;

// Transition lookup table
static const int8_t TRANSITION_TABLE[16] = {0,  -1, 1, 0, 1, 0, 0,  -1,
                                            -1, 0,  0, 1, 0, 1, -1, 0};

void Dial::init() {
  // Inputs and pullups
  ENC_DDR &= ~((1 << ENC_CLK) | (1 << ENC_DT) | (1 << ENC_SW));
  ENC_PORT |= (1 << ENC_CLK) | (1 << ENC_DT) | (1 << ENC_SW);

  // Read initial state
  uint8_t clk = (ENC_PIN & (1 << ENC_CLK)) ? 1 : 0;
  uint8_t dt = (ENC_PIN & (1 << ENC_DT)) ? 1 : 0;
  prev_state = (clk << 1) | dt;

  // INT0 and INT1 = any logical change
  EICRA |= (1 << ISC00);
  EICRA |= (1 << ISC10);
  // Enable interrupts
  EIMSK |= (1 << INT0) | (1 << INT1);
}

void handleInterrupt() {
  uint8_t clk = (ENC_PIN & (1 << ENC_CLK)) ? 1 : 0;
  uint8_t dt = (ENC_PIN & (1 << ENC_DT)) ? 1 : 0;
  uint8_t current = (clk << 1) | dt;
  uint8_t index = (prev_state << 2) | current;
  int8_t transition = TRANSITION_TABLE[index];

  accumulator += transition;
  prev_state = current;

  // One full detent
  if (accumulator >= 4) {
    pending_movement++;
    accumulator = 0;
  }

  if (accumulator <= -4) {
    pending_movement--;
    accumulator = 0;
  }
}

int8_t Dial::poll() {
  int8_t movement;

  uint8_t sreg = SREG;
  cli();
  movement = pending_movement;
  pending_movement = 0;
  SREG = sreg;

  return movement;
}

uint8_t Dial::pressed() { return !(ENC_PIN & (1 << ENC_SW)); }

ISR(INT0_vect) { handleInterrupt(); }

ISR(INT1_vect) { handleInterrupt(); }