#include "timer.hpp"
#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint32_t timer0_overflow_count = 0;
volatile uint32_t timer0_millis = 0;

ISR(TIMER0_OVF_vect) {
  // There is a 24us drift per ms, but the accuracy is good enough.
  timer0_overflow_count++;
  timer0_millis += 1;
}

void Timer::init() {
  TCCR0A = 0;                          // Normal mode
  TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64
  TIMSK0 = (1 << TOIE0);               // Enable overflow interrupt
  sei();                               // Enable interrupts
}

uint32_t Timer::millis() {
  uint32_t t;
  cli();
  t = timer0_millis;
  sei();
  return t;
}

uint32_t Timer::micros() {
  uint32_t overflows;
  uint8_t t;

  // Guarantee atomicity
  cli();
  overflows = timer0_overflow_count;
  t = TCNT0;
  if ((TIFR0 & (1 << TOV0)) && (t < 255)) overflows++;
  sei();

  return ((overflows << 8) + t) * 4;
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
    while (cycles--) {
      asm volatile("nop");
    }
  }
}

uint32_t millis() { return Timer::millis(); }

uint32_t micros() { return Timer::micros(); }

void delay(uint32_t ms) { Timer::delay(ms); }

void delayMicroseconds(uint32_t us) { Timer::delayMicroseconds(us); }