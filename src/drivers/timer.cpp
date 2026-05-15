#include <avr/interrupt.h>
#include <avr/io.h>

#include "drivers/timer.hpp"

volatile uint32_t timer0_millis = 0;

ISR(TIMER0_COMPA_vect) { timer0_millis += 1; }

void Timer::init() {
  TCCR0A = (1 << WGM01);               // CTC mode
  TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64
  OCR0A = 249;                         // 250 ticks @ 250kHz = 1ms
  TIMSK0 = (1 << OCIE0A);              // Enable compare interrupt
  sei();                               // Enable interrupts
}

uint32_t Timer::millis() {
  uint32_t t;

  uint8_t sreg = SREG;
  cli();
  t = timer0_millis;
  SREG = sreg;

  return t;
}

uint32_t Timer::micros() {
  uint32_t ms;
  uint8_t t;
  uint8_t sreg = SREG;

  cli();
  ms = timer0_millis;
  t = TCNT0;

  // Pending compare match
  if ((TIFR0 & (1 << OCF0A)) && t < OCR0A) { ms++; }
  SREG = sreg;

  // Timer tick:
  // 16MHz / 64 = 250kHz
  // 1 tick = 4us
  return (ms * 1000ul) + (t * 4ul);
}

void Timer::delay(uint32_t ms) {
  uint32_t start = millis();
  while ((millis() - start) < ms);
}

void Timer::delayMicroseconds(uint32_t us) {
  // For micros, we need a much more precise implementation for timekeeping, as
  // using the coarse interrupt method will easily result in times over 10us.
  // For microsecond accuracy, we need to go down to a clock cycle level. At
  // 16 MHz, each cycle takes ~63ns to complete, so ~16 CPU cycles are needed to
  // delay for 1us. This can be extended to delay for specified microseconds. As
  // delays are blocking, a busy loop will work perfectly for this use case.
  while (us--) {
    uint8_t cycles = 16;
    while (cycles--) { asm volatile("nop"); }
  }
}

uint32_t millis() { return Timer::millis(); }

uint32_t micros() { return Timer::micros(); }

void delay(uint32_t ms) { Timer::delay(ms); }

void delayMicroseconds(uint32_t us) { Timer::delayMicroseconds(us); }