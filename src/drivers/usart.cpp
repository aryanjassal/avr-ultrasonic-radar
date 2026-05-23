#include "drivers/usart.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

static void (*usartRxHandler)() = nullptr;

// Use a ring buffer with 32 bytes of storage to queue up data to send. Allows
// for cheap and efficient queueing.
static constexpr uint8_t TX_BUFFER_SIZE = 32;
static volatile char txBuffer[TX_BUFFER_SIZE];
static volatile uint8_t txHead = 0;
static volatile uint8_t txTail = 0;

ISR(USART_RX_vect) {
  if (usartRxHandler != nullptr) {
    usartRxHandler();
  } else {
    (void)UDR0;
  }
}

ISR(USART_UDRE_vect) {
  // Buffer empty?
  if (txHead == txTail) {
    // Disable interrupt
    UCSR0B &= ~(1 << UDRIE0);
    return;
  }

  // Transmit next byte
  UDR0 = txBuffer[txTail];
  txTail = (txTail + 1) % TX_BUFFER_SIZE;
}

void USART::init(uint32_t baud, bool doubleSpeed) {
  UCSR0A = doubleSpeed ? (1 << U2X0) : 0;
  UBRR0 = (uint16_t)(F_CPU / (doubleSpeed ? 8ul : 16ul * baud) - 1);
  UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART::flush() {
  while ((UCSR0A & (1 << RXC0))) (void)UDR0;
}

void USART::print(char c) {
  uint8_t next = (txHead + 1) % TX_BUFFER_SIZE;

  // Wait if full
  while (next == txTail);

  txBuffer[txHead] = c;
  txHead = next;

  // Enable TX interrupt
  UCSR0B |= (1 << UDRIE0);
}

void USART::print(const char* str) {
  while (*str) print(*str++);
}

void USART::println(const char* str) {
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

void USART::installRx(void (*handler)()) {
  usartRxHandler = handler;
  UCSR0B |= (1 << RXCIE0);
}
