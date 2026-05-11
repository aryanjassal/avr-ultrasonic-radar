#pragma once

#include <avr/io.h>
#include <stdint.h>

namespace Pins {
  void int0Init();
}

struct PinDescriptor {
  volatile uint8_t *ddr;
  volatile uint8_t *port;
  volatile uint8_t *pin;
  uint8_t bit;
};

class OutputPin {
  volatile uint8_t *mPort;
  uint8_t mMask;

public:
  OutputPin(const PinDescriptor &d);

  void write(bool high);
  void high();
  void low();
  void toggle();
};

class InputPin {
  volatile uint8_t *mPort;
  volatile uint8_t *mPin;
  uint8_t mMask;

public:
  explicit InputPin(const PinDescriptor &d, bool pullup = true);

  void setPullup(bool enable);

  // Uses PCINTs because requirement said so.
  bool read() const;

  // Uses PIN read in case PCINT doesn't work
  bool readRaw() const;
};

class AdcPin {
  uint8_t mChannel;

public:
  AdcPin(const PinDescriptor &d);

  // Actual value is 10-bit (0-1023)
  uint16_t read();
};

class PwmPin {
  volatile uint8_t *mOcr;
  OutputPin mPin;

public:
  PwmPin(const PinDescriptor &pin, volatile uint8_t *ocr);

  void write(uint8_t duty);
};

extern const PinDescriptor D0; // PD0
extern const PinDescriptor D1; // PD1
extern const PinDescriptor D2; // PD2
extern const PinDescriptor D3; // PD3
extern const PinDescriptor D4; // PD4
extern const PinDescriptor D5; // PD5
extern const PinDescriptor D6; // PD6
extern const PinDescriptor D7; // PD7

extern const PinDescriptor D8;  // PB0
extern const PinDescriptor D9;  // PB1
extern const PinDescriptor D10; // PB2
extern const PinDescriptor D11; // PB3
extern const PinDescriptor D12; // PB4
extern const PinDescriptor D13; // PB5

extern const PinDescriptor A0; // PC0
extern const PinDescriptor A1; // PC1
extern const PinDescriptor A2; // PC2
extern const PinDescriptor A3; // PC3
extern const PinDescriptor A4; // PC4
extern const PinDescriptor A5; // PC5

// Timer1 (16-bit)
extern const PinDescriptor &PWM_OC1A;
extern const PinDescriptor &PWM_OC1B;

//  Timer2 (8-bit)
extern const PinDescriptor &PWM_OC2A;
extern const PinDescriptor &PWM_OC2B;

// External interrupts (INT0 and INT1)
extern const PinDescriptor &PIN_INT0;
extern const PinDescriptor &PIN_INT1;

// USART
extern const PinDescriptor &USART_RX;
extern const PinDescriptor &USART_TX;

// Built-in LED
extern const PinDescriptor &LED_BUILTIN;