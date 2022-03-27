# at89s4051-prog
ISP programmer for at89s4051 microcontrollers, on Arduino

Atmel's (Microchip) family of 8051-clones includes AT89S4051 with 4k flash. They allow In-System Programming (enabled by default).

This project allows sending Intel HEX file of compiled 8051 code over UART to Arduino, which further uploads it via SPI to
attached AT89S4051. Probably also should work for AT89S52.

### Wiring

- pin 9 - to target reset (1)
- pin 10 - to target MOSI (17)
- pin 11 - to target MISO (18)
- pin 12 - to target CLK (19)

Attach VCC and GND to target (20 and 10 respectively), plug in quartz between 4 and 5. Optionally P1.0, P1.1 could be
pulled up with external resistors (others have internal pull-ups).

### UART settings

1200 Baud (to match firmware writing speed), no parity, single stop bit.
