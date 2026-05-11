#define F_CPU 16000000ul
#include "usart.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

static void (*usartRxHandler)() = nullptr;

ISR(USART_RX_vect) {
  if (usartRxHandler != nullptr) {
    usartRxHandler();
  }
}

void USART::init(uint32_t baud) {
  UCSR0A = 0;
  UBRR0 = (uint16_t)(F_CPU / (16ul * baud) - 1);
  UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART::flush() {
  while ((UCSR0A & (1 << RXC0)))
    (void)UDR0;
}

void USART::print(char c) {
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = c;
}

void USART::print(const char *str) {
  while (*str)
    print(*str++);
}

void USART::println(const char *str) {
  print(str);
  print("\r\n");
}

void USART::println() { print("\r\n"); }

void USART::print_uint32(uint32_t num) {
  char buf[11];
  ultoa(num, buf, 10);
  USART::print(buf);
}

void USART::print_int32(int32_t num) {
  char buf[12];
  ltoa(num, buf, 10);
  USART::print(buf);
}

void USART::print_float(float num, uint8_t width, uint8_t precision) {
  char buf[12];
  dtostrf(num, width, precision, buf);
  USART::print(buf);
}

void USART::installRx(void (*handler)()) { usartRxHandler = handler; }