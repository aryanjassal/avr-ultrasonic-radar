#include "drivers/pin.hpp"

// ADC

void Analog::init() {
  ADMUX = (1 << REFS0);  // AVcc reference
  // Enable ADC, prescaler 128
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Output pins

OutputPin::OutputPin(const PinDescriptor& d)
    : mPort(d.port), mMask(1 << d.bit) {
  *d.ddr |= mMask;    // Set output
  *d.port &= ~mMask;  // Default low
}

void OutputPin::write(bool high) {
  if (high) {
    *mPort |= mMask;
  } else {
    *mPort &= ~mMask;
  }
}

void OutputPin::high() { *mPort |= mMask; }

void OutputPin::low() { *mPort &= ~mMask; }

void OutputPin::toggle() { *mPort ^= mMask; }

// Input pins

InputPin::InputPin(const PinDescriptor& d, bool pullup)
    : mPort(d.port), mPin(d.pin), mMask(1 << d.bit) {
  *d.ddr &= ~mMask;  // Set input
  // clang-format off
  if (pullup) *mPort |= mMask;  // Enable pull-up
  else *mPort &= ~mMask;        // Floating
  // clang-format on
}

bool InputPin::read() const { return (*mPin & mMask) != 0; }

// ADC pins

AnalogPin::AnalogPin(const PinDescriptor& d) : mChannel(d.bit) {}

uint16_t AnalogPin::read() {
  ADMUX = (ADMUX & 0xf0) | (mChannel & 0x0f);  // Select channel
  ADCSRA |= (1 << ADSC);                       // Start conversion
  while (ADCSRA & (1 << ADSC));                // Wait
  return ADC;
}

// Pin descriptors

const PinDescriptor D0 = {&DDRD, &PORTD, &PIND, PD0};
const PinDescriptor D1 = {&DDRD, &PORTD, &PIND, PD1};
const PinDescriptor D2 = {&DDRD, &PORTD, &PIND, PD2};
const PinDescriptor D3 = {&DDRD, &PORTD, &PIND, PD3};
const PinDescriptor D4 = {&DDRD, &PORTD, &PIND, PD4};
const PinDescriptor D5 = {&DDRD, &PORTD, &PIND, PD5};
const PinDescriptor D6 = {&DDRD, &PORTD, &PIND, PD6};
const PinDescriptor D7 = {&DDRD, &PORTD, &PIND, PD7};

const PinDescriptor D8 = {&DDRB, &PORTB, &PINB, PB0};
const PinDescriptor D9 = {&DDRB, &PORTB, &PINB, PB1};
const PinDescriptor D10 = {&DDRB, &PORTB, &PINB, PB2};
const PinDescriptor D11 = {&DDRB, &PORTB, &PINB, PB3};
const PinDescriptor D12 = {&DDRB, &PORTB, &PINB, PB4};
const PinDescriptor D13 = {&DDRB, &PORTB, &PINB, PB5};

const PinDescriptor A0 = {&DDRC, &PORTC, &PINC, PC0};
const PinDescriptor A1 = {&DDRC, &PORTC, &PINC, PC1};
const PinDescriptor A2 = {&DDRC, &PORTC, &PINC, PC2};
const PinDescriptor A3 = {&DDRC, &PORTC, &PINC, PC3};
const PinDescriptor A4 = {&DDRC, &PORTC, &PINC, PC4};
const PinDescriptor A5 = {&DDRC, &PORTC, &PINC, PC5};
