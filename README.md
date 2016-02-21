# SaturnJVS

A project to build a replacement JVS I/O board for a NAOMI arcade board which uses Sega Saturn controllers as input.

## Details

This relies on two boards which communicate by I2C. One board handles JVS communication with the NAOMI while the other handles controller input from Sega Saturn controllers. Originally, the plan was to support different types of controllers by swapping controller input boards.

Both boards rely on Atmel AVR ATmega328P microcontrollers and the firmware is written using the Arduino framework. The boards, however, are custom made and not arduino boards (though Arduino boards might be usable as well).

## Status

Using standard Sega Saturn controllers to play games on a NAOMI 2 board works well. There are some missing features including analog pad support and button remapping.

This project is on hold.

## Directory Layout

- `eagle`: Eagle schematics and board layout data.
- `naomi_io`: Firmware for the JVS communication board.
- `saturn_js`: Firmware for the Sega Saturn controller board.
