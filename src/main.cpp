// #define F_CPU 16000000ul

// #include <avr/interrupt.h>
// #include <avr/io.h>
// #include <util/delay.h>

// #include "pin.hpp"
// #include "servo.hpp"
// #include "timer.hpp"
// #include "ultrasonic.hpp"
// #include "usart.hpp"

// #define POINTS 6

// int main(void) {
//   Timer::init();
//   Servo::init();
//   USART::init(9600);
//   Ultrasonic sensor(D2, D3);

//   sei();

//   // Integer angle spacing
//   const uint8_t step = (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / (POINTS - 1);

//   while (true) {
//     for (uint8_t i = 0; i < POINTS; i++) {
//       uint8_t angle = SERVO_MIN_ANGLE + (i * step);
//       Servo::write_angle(angle);
//       delay(60);
//       sensor.trigger();
//       delay(50);
//       uint32_t distance = sensor.distance_mm();

//       // Debug logging
//       USART::print_uint32(angle);
//       USART::print(":");
//       USART::print_uint32(distance);
//       USART::println();
//     }
//     for (int8_t i = POINTS - 2; i > 0; i--) {
//       uint8_t angle = SERVO_MIN_ANGLE + (i * step);
//       Servo::write_angle(angle);
//       delay(60);
//       sensor.trigger();
//       delay(50);
//       uint32_t distance = sensor.distance_mm();

//       // Debug logging
//       USART::print_uint32(angle);
//       USART::print(":");
//       USART::print_uint32(distance);
//       USART::println();
//     }
//   }
// }

#include <avr/io.h>
#include <avr/interrupt.h>

// // #include "dial.hpp"
// #include "usart.hpp"
#include "drivers/lcd.hpp"
#include "pin.hpp"
#include "timer.hpp"

int main(void) {
  Timer::init();
  LCD display(D2, D3, D4, D5, D6, D7);
  // Dial::init();
  // USART::init(9600);


  // int value = 0;

  display.print("Hello world");
  while (true) {
    // int8_t delta = Dial::poll();

    // if (delta) { value += delta; }

    // if (Dial::pressed()) { USART::println("PRESSED"); }
    // USART::print_int32(value);
    // USART::println();
  }
}